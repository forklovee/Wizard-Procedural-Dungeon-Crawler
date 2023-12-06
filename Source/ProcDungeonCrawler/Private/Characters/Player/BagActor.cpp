// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/BagActor.h"

#include "Components/WidgetComponent.h"
#include "Components/Character/BagComponent.h"
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

void ABagActor::Open(UBagComponent* BagComponent)
{
	Bag = BagComponent;
	BagUI = Cast<UBagUI>(BagWidget->GetWidget());
	GridSize = BagComponent->GetBagGridSize();

	// Handles item tiles and actors spawn
	BagItemTiles = BagUI->Setup(this, BagComponent);

	for (UBagItemTile* BagItemTile: BagItemTiles)
	{
		if (BagItemTile == nullptr) continue;

		SpawnItemActor(BagItemTile);
	}
}

void ABagActor::Close()
{
	// Reset all variables
	Bag = nullptr;
	GrabbedItemActor = nullptr;
	HoveredItemTile = nullptr;
	LastHoveredItemTile = nullptr;

	// Destroy all spawned items
	for (UBagItemTile* BagItemTile : BagItemTiles)
	{
		if (BagItemTile == nullptr) continue;
		DestroyItemActor(BagItemTile);
	}
	BagItemTiles.Empty();
}

// Utility
FVector ABagActor::GetSlotLocation(const FBagSlot* BagSlot) const
{
	if (BagSlot == nullptr)
	{
		return FVector::ZeroVector;
	}
	const float ItemActorsOffset = 23.0f;
	return BagWidget->GetComponentLocation() -
		(BagWidget->GetForwardVector() * 50.f) +
		(FVector::UpVector * GridSize.Y * .5f * -12.f) +
		(BagWidget->GetRightVector() * GridSize.X * .5f * 18.f) +
		-(BagWidget->GetRightVector() * ItemActorsOffset * BagSlot->TilePos.X);
}

UBagItemTile* ABagActor::GetBagItemTileByItemClass(TSubclassOf<APickupItem> ItemClass)
{
	for (UBagItemTile* BagItemTile: BagItemTiles)
	{
		if (BagItemTile->GetBagSlot() == nullptr) continue;
		
		if (BagItemTile->GetBagSlot()->ItemClass == ItemClass)
		{
			return BagItemTile;
		}
	}
	return nullptr;
}

void ABagActor::OnTargetTileChanged(UBagItemTile* HoveredTile, bool bIsHovered)
{
	if (GrabbedItemData.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No grabbed item data!"))
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Hovered Tile: %s"), *HoveredTile->GetName())
	
	if (HoveredItemTile == HoveredTile && !bIsHovered)
	{
		HoveredItemTile = nullptr;
		return;
	}

	FBagSlot* HoveredBagSlot = HoveredTile->GetBagSlot();
	if (bIsHovered)
	{
		HoveredBagSlot->ItemClass = GrabbedItemData.ItemClass;
		HoveredBagSlot->Amount = GrabbedItemData.Amount;
		HoveredTile->UpdateVisualData();
	}
	else
	{
		if (LastHoveredItemTile.IsValid())
		{
			FBagSlot* HoveredBagSlot_LastItemTile = LastHoveredItemTile->GetBagSlot();
			HoveredBagSlot_LastItemTile->ItemClass = LastHoveredItemTile->LastBagSlotData.ItemClass;
			HoveredBagSlot_LastItemTile->Amount = LastHoveredItemTile->LastBagSlotData.Amount;
			LastHoveredItemTile->UpdateVisualData();
		}
		LastHoveredItemTile = HoveredTile;
	}
	
	LastHoveredItemTile = HoveredItemTile;
	HoveredItemTile = HoveredTile;
}

// Item actors management
void ABagActor::SpawnItemActor(UBagItemTile* ItemTile)
{
	if (ItemTile == nullptr || ItemTile->GetBagSlot() == nullptr ||
		ItemTile->GetBagSlot()->ItemClass == nullptr)
	{
		return;
	}
	
	const FBagSlot* BagSlot = ItemTile->GetBagSlot();
	const TSubclassOf<APickupItem> ItemClass = BagSlot->ItemClass;

	const FVector ItemLocation = GetSlotLocation(BagSlot);
	const FRotator ItemRotation = GetActorRotation();

	APickupItem* PickupItemActor = Cast<APickupItem>(
			GetWorld()->SpawnActor(ItemClass, &ItemLocation, &ItemRotation)
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
	if (ItemTile == nullptr || ItemTile->GetBagSlot() == nullptr ||
		ItemTile->GetBagSlot()->ItemClass == nullptr)
	{
		return;
	}
	
	if (APickupItem* PickupItemActor = ItemTile->GetPickupItemActor())
	{
		UE_LOG(LogTemp, Display, TEXT("ItemTile: %s Actor Destroyed!"), *ItemTile->GetBagSlot()->ItemClass->GetName())

		PickupItemActor->Destroy();
		ItemTile->SetPickupItemActor(nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemTile: %s DestroyItemActor: NULL!"), *ItemTile->GetBagSlot()->ItemClass->GetName())
	}
}

void ABagActor::OnNewItemAdded(FBagSlot Slot)
{
}

void ABagActor::OnItemRemoved(FBagSlot Slot)
{
}

// Utility


// Handle player hand interactions
void ABagActor::BeginItemGrab(UPrimitiveComponent* GrabComponent, AActor* Actor)
{
	if (!GrabComponent || !Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("BeginItemGrab: GrabComponent or Actor is nullptr!"))
		return;
	}

	const TSubclassOf<APickupItem> ItemClass = Actor->GetClass();
	if (ItemClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaginItemGrab: ItemClass is not a Pickup Item!"))
		return;
	}

	GrabbedItemData = FBagSlot(ItemClass, 1);
	
	// Register hover events
	for (UBagItemTile* BagItemTile: BagItemTiles)
	{
		BagItemTile->SetCanBeTargeted(true);
		BagItemTile->OnHoverChanged.AddDynamic(this, &ABagActor::OnTargetTileChanged);
	}

	// Grabbed item out of inventory
	if (Bag->HasItem(ItemClass))
	{
		UBagItemTile* GrabbedItemTile = GetBagItemTileByItemClass(ItemClass);
		GrabbedItemData = *GrabbedItemTile->GetBagSlot();
		
		Bag->RemoveItem(ItemClass, GrabbedItemData.Amount);
		GrabbedItemTile->UpdateVisualData();
	}
	
	GrabbedItemActor = Cast<APickupItem>(Actor);
}

void ABagActor::OnPlayerCursorHoverChanged(bool bIsHovered, AActor* PlayerGrabbedItem)
{
	if (!BagUI.IsValid())
	{
		return;
	}
	if (!GrabbedItemActor.IsValid() || GrabbedItemData.IsEmpty())
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Cursor Hover Changed: %i"), bIsHovered);

	// Remove from inventory
	if (!bIsHovered)
	{
		if (OnGrabbedActorPositionOverrideClearRequest.IsBound())
		{
			OnGrabbedActorPositionOverrideClearRequest.Broadcast();
		}
		return;
	}
	
	// Add to inventory
	if (HoveredItemTile.IsValid())
	{
		if (HoveredItemTile.IsValid() && OnGrabbedActorPositionOverrideRequest.IsBound())
		{
			OnGrabbedActorPositionOverrideRequest.Broadcast(
				GetSlotLocation(HoveredItemTile->GetBagSlot())
				);
		}
	}

	// // Mark item for removal if it's outside of bag ui
	// bRemoveGrabbedItem = !bIsHovered;
}

void ABagActor::CommitGrabbedItemAction(UPrimitiveComponent* GrabComponent, AActor* Actor)
{
	// Unregister hover event
	for (UBagItemTile* BagItemTile: BagItemTiles)
	{
		BagItemTile->SetCanBeTargeted(false);
		BagItemTile->OnHoverChanged.RemoveDynamic(this, &ABagActor::OnTargetTileChanged);
	}
	HoveredItemTile = nullptr;
	
	// if (!GrabbedItemActor.IsValid())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Commit: GrabbedItemActor is nullptr!"))
	// 	return;
	// }
	// const TSubclassOf<APickupItem> ItemClass = GrabbedItemActor.Get()->GetClass();
	// if (ItemClass == nullptr)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Commit: ItemClass is nullptr!"))
	// 	return;
	// }
	//
	// if (!bGrabbedItemHoverActionChanged || bRemoveGrabbedItemInitial != bRemoveGrabbedItem)
	// {
	// 	// No action
	// 	UE_LOG(LogTemp, Warning, TEXT("No action"))
	// 	// If item was from inventory, return it to position
	// 	if (bGrabbedItemHoverActionChanged && !bRemoveGrabbedItem)
	// 	{
	// 		if (const APickupItem* PickupItem = Cast<APickupItem>(GrabbedItemActor.Get()))
	// 		{
	// 			Actor->Destroy();
	//
	// 			if (UBagItemTile* LastBagItemTile = GetBagItemTileByItemClass(ItemClass))
	// 			{
	// 				SpawnItemActor(LastBagItemTile);
	// 			}
	// 		}
	// 	}
	// }
	// else if (!bRemoveGrabbedItem)
	// {
	// 	// Add Item
	// 	if (const APickupItem* PickupItem = Cast<APickupItem>(Actor))
	// 	{
	// 		if (OnAddItemRequest.IsBound())
	// 		{
	// 			OnAddItemRequest.Broadcast(PickupItem->GetClass());
	// 		}
	// 		Actor->Destroy();
	// 	}
	// }
	// else
	// {
	// 	// Remove Item
	// 	if (const APickupItem* PickupItem = Cast<APickupItem>(Actor))
	// 	{
	// 		if (OnRemoveItemRequest.IsBound())
	// 		{
	// 			OnRemoveItemRequest.Broadcast(PickupItem->GetClass());
	// 		}
	// 	}
	// }
	//
	// GrabbedItemActor = nullptr;
	//
	// bGrabbedItemHoverActionChanged = false;
	// bRemoveGrabbedItemInitial = false;
	// bRemoveGrabbedItem = false;
}