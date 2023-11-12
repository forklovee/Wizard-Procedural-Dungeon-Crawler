// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/BagActor.h"

#include "Components/WidgetComponent.h"
#include "Items/PickupItem.h"
#include "UI/Bag/BagItemTile.h"
#include "UI/Bag/BagUI.h"

ABagActor::ABagActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));;

	BagMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BagMesh"));
	BagMesh->SetupAttachment(RootComponent);
	
	SlotsRootComponent = CreateDefaultSubobject<USceneComponent>(FName("SlotsRootComponent"));
	SlotsRootComponent->SetupAttachment(BagMesh);
	
	BagWidget = CreateDefaultSubobject<UWidgetComponent>(FName("BagWidget"));
	BagWidget->SetupAttachment(BagMesh);
	BagWidget->SetManuallyRedraw(true);
}
void ABagActor::BeginPlay()
{
	Super::BeginPlay();
	
	BagUI = Cast<UBagUI>(BagWidget->GetWidget());
	BagUI->OnNewItemTileCreated.AddDynamic(this, &ABagActor::ItemTile_SpawnAndDestroyRequests);
}

// Setup
void ABagActor::SetupView()
{
	if (!BagUI.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Bag UI not valid!"))
		return;
	}
	
	BagItemTiles = BagUI->SetupBagUI(this, PawnItems);
	BagUI->ChangePage(0);
}

void ABagActor::ClearView()
{
	if (!BagUI.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Bag UI not valid!"))
		return;
	}
	for (APickupItem* PickupItem : BagUI->GetAllSpawnedActors())
	{
		PickupItem->Destroy();
	}
}

void ABagActor::SetPawnItems(TMap<TSubclassOf<APickupItem>, int32>& Items)
{
	PawnItems = &Items;
}
//Setup

// Utility
FVector ABagActor::GetSlotLocation(int SlotIdx) const
{
	const int ViewSlotIdx = SlotIdx % 3;
	const FVector RightVector = GetActorForwardVector().RotateAngleAxis(-90.f, FVector::UpVector);
	const FVector SlotsOrigin = BagWidget->GetComponentLocation() - (RightVector * SlotOffset);
	return SlotsOrigin + (RightVector * SlotOffset * ViewSlotIdx);
	// return BagWidget->GetComponentLocation() + RightVector;
}

void ABagActor::ItemTile_SpawnAndDestroyRequests(UBagItemTile* ItemTile)
{
	if (ItemTile == nullptr)
	{
		return;
	}
	
	ItemTile->OnSpawnPickupItemRequest.AddDynamic(this, &ABagActor::SpawnItemActor);
	ItemTile->OnDestroyPickupItemRequest.AddDynamic(this, &ABagActor::DestroyItemActor);
}

void ABagActor::SpawnItemActor(UBagItemTile* ItemTile)
{
	if (ItemTile == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnItemActor: ItemTile is nullptr!"))
		return;
	}
	const int32 ItemIndex = 0; //TODO: Get VISIBLE PAGE index
	const TSubclassOf<APickupItem> ItemClass = *ItemTile->GetPickupItemClass();
	const FVector ItemLocation = GetSlotLocation(ItemIndex);
	const FRotator ItemRotation = FRotator::ZeroRotator;

	APickupItem* PickupItemActor = Cast<APickupItem>(
			GetWorld()->SpawnActor(ItemClass->GetDefaultObject()->GetClass(), &ItemLocation, &ItemRotation)
			);
	PickupItemActor->SetSimulatePhysics(false);

	if (!ItemTile->bItemActorGrabChangeEventConnected)
	{
		ItemTile->bItemActorGrabChangeEventConnected = true;
	}
	
	ItemTile->SetPickupItemActor( PickupItemActor );
}

void ABagActor::DestroyItemActor(UBagItemTile* ItemTile)
{
	if (APickupItem* PickupItemActor = ItemTile->GetPickupItemActor())
	{
		PickupItemActor->Destroy();
		ItemTile->SetPickupItemActor(nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DestroyItemActor: NULL!"))
	}
}
// Utility

// Item management
void ABagActor::BeginItemGrab(UPrimitiveComponent* GrabComponent, AActor* Actor)
{
	GrabbedItemActor = Actor;

	bGrabbedItemHoverActionChanged = false;
	bRemoveGrabbedItemInitial = false;
	bRemoveGrabbedItem = false;
}

void ABagActor::OnPlayerCursorHoverChanged(bool bIsHovered, AActor* PlayerGrabbedItem)
{
	if (!BagUI.IsValid())
	{
		return;
	}
	if (!GrabbedItemActor.IsValid())
	{
		return;
	}

	if (!bGrabbedItemHoverActionChanged)
	{
		bGrabbedItemHoverActionChanged = true;
		bRemoveGrabbedItemInitial = !bIsHovered;
	}
	
	// Mark item for removal if it's outside of bag ui
	bRemoveGrabbedItem = !bIsHovered;
	
	if (bIsHovered)
	{
		if (OnGrabbedActorPositionOverrideRequest.IsBound())
		{
			OnGrabbedActorPositionOverrideRequest.Broadcast( GetSlotLocation(0) + FVector3d::UpVector*50.f); //TODO: Get VISIBLE PAGE index
		}
	}
	else
	{
		if (OnGrabbedActorPositionOverrideClearRequest.IsBound())
		{
			OnGrabbedActorPositionOverrideClearRequest.Broadcast();
		}
	}
}

void ABagActor::CommitGrabbedItemAction(UPrimitiveComponent* GrabComponent, AActor* Actor)
{
	if (!GrabbedItemActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Commit: GrabbedItemActor is nullptr!"))
		return;
	}

	if (!bGrabbedItemHoverActionChanged || bRemoveGrabbedItemInitial != bRemoveGrabbedItem)
	{
		// No action
		UE_LOG(LogTemp, Warning, TEXT("No action"))
		// If item was form inventory, return it to position
		if (bGrabbedItemHoverActionChanged && !bRemoveGrabbedItem)
		{
			if (const APickupItem* PickupItem = Cast<APickupItem>(Actor))
			{
				SpawnItemActor(BagItemTiles.FindRef(PickupItem->GetClass()));
				Actor->Destroy();
			}
		}
	}
	else if (!bRemoveGrabbedItem)
	{
		// Add Item
		if (const APickupItem* PickupItem = Cast<APickupItem>(Actor))
		{
			if (OnAddItemRequest.IsBound())
			{
				OnAddItemRequest.Broadcast(PickupItem->GetClass());
			}
			Actor->Destroy();
		}
	}
	else
	{
		// Remove Item
		if (const APickupItem* PickupItem = Cast<APickupItem>(Actor))
		{
			if (OnRemoveItemRequest.IsBound())
			{
				OnRemoveItemRequest.Broadcast(PickupItem->GetClass());
			}
		}
	}

	GrabbedItemActor = nullptr;
	
	bGrabbedItemHoverActionChanged = false;
	bRemoveGrabbedItemInitial = false;
	bRemoveGrabbedItem = false;
}

void ABagActor::OnNewItemAdded(TSubclassOf<APickupItem> ItemClass, int32 NewAmount)
{
	if (!BagUI.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Bag UI not valid!"))
		return;
	}

	if (NewAmount == 1)
	{
		if (UBagItemTile* NewItemTile = BagUI->CreateNewItemTile(ItemClass, NewAmount))
		{
			BagItemTiles.Add(ItemClass, NewItemTile);
			BagUI->ChangePage(1000);
		}
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("BagItemTiles NUM: %i"), BagItemTiles.Num());
	if (UBagItemTile* ItemTile = BagItemTiles.FindRef(ItemClass))
	{
		ItemTile->SetItemAmountTextBlock(NewAmount);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemTile not found!!!"))
	}
}

void ABagActor::OnItemRemoved(TSubclassOf<APickupItem> ItemClass, int32 NewAmount)
{
	UBagItemTile* ItemTile = BagItemTiles.FindRef(ItemClass);
	if (ItemTile == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OnItemRemoved: ItemTile not found!") );
		return;
	}
	
	// Remove Item Tile
	if (NewAmount <= 0)
	{
		BagItemTiles.Remove(ItemClass);
		ItemTile->SetPickupItemClass(nullptr);
		return;
	}
	
	// Update Item Tile Amount
	ItemTile->SetItemAmountTextBlock(NewAmount);
	ItemTile->SpawnPickupItem();
}
// Item management

// Input
void ABagActor::ChangeSlotsPage(int PageOffset)
{
	if (!BagUI.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Bag UI not valid!"))
		return;
	}
	BagUI->ChangePage(PageOffset);
}
