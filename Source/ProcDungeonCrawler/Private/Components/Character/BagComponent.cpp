// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/BagComponent.h"

#include "IDetailTreeNode.h"
#include "Components/WidgetComponent.h"
#include "Items/PickupItem.h"
#include "Characters/Player/BagActor.h"
#include "Characters/Player/WizardPlayer.h"
#include "Components/Character/PlayerInteractionRaycast.h"

UBagComponent::UBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBagComponent::OnPlayerCursorOnWidgetHoverChanged(UWidgetComponent* WidgetComponent,
	UWidgetComponent* PreviousWidgetComponent)
{
	const bool NewPlayerCursorInBounds = WidgetComponent != nullptr && PreviousWidgetComponent == nullptr;
	if (OnPlayerCursorHoverChanged.IsBound() && NewPlayerCursorInBounds != bPlayerCursorInBounds && BagOwner.IsValid())
	{
		OnPlayerCursorHoverChanged.Broadcast(NewPlayerCursorInBounds, BagOwner->PlayerInteraction->GetGrabbedActor());
	}
	bPlayerCursorInBounds = NewPlayerCursorInBounds;
}

bool UBagComponent::IsOpen() const
{
	return BagActor.IsValid();
}


void UBagComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AWizardPlayer* WizardPawn = Cast<AWizardPlayer>(GetOwner()))
	{
		BagOwner = WizardPawn;
	}

	for (int TileIdx = 0; TileIdx < GetBagSize(); TileIdx++)
	{
		const int ColIdx = TileIdx % (int)BagGridSize.X;
		const int RowIdx = FMath::FloorToInt(TileIdx / (BagGridSize.X*.1f));
		Items.Add(FBagSlot(TileIdx, FVector2D(ColIdx, RowIdx)));
	}
}

void UBagComponent::UpdateItemSlot(FBagSlot BagSlotOverride)
{
	if (BagSlotOverride.Index == -1) return;

	FBagSlot& InventoryBagSlot = Items[BagSlotOverride.Index];
	InventoryBagSlot.Amount = BagSlotOverride.Amount;
	InventoryBagSlot.ItemClass = BagSlotOverride.ItemClass;
}

void UBagComponent::SetBagActorAttached(const bool bIsAttached)
{
	if (!BagActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to set Bag Actor attachment (actor is null)!"));
		return;
	}
	
	if (bIsAttached)
	{
		BagActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
	}
	else
	{
		BagActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void UBagComponent::ToggleBag()
{
	if (BagActor.IsValid())
	{
		OnPlayerCursorHoverChanged.RemoveDynamic(BagActor.Get(), &ABagActor::OnPlayerCursorHoverChanged);
		BagActor->Close();

		// Destroy bag actor
		BagActor.Get()->Destroy();
		BagActor = nullptr;
		bPlayerCursorInBounds = false;
	}
	else
	{
		if (BagActorClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Bag Actor Class is null!"));
			return;
		}
		
		const FVector BagLocation = GetComponentLocation();
		FRotator BagRotation = GetComponentRotation();
		BagRotation.Yaw += 180.0f;
		
		BagActor = Cast<ABagActor>(GetWorld()->SpawnActor(BagActorClass, &BagLocation, &BagRotation));
		if (BagActor.IsValid())
		{
			BagActor->Open(this);

			BagActor->OnAddItemRequest.AddDynamic(this, &UBagComponent::UpdateItemSlot);
			BagActor->OnRemoveItemRequest.AddDynamic(this, &UBagComponent::UpdateItemSlot);
			
			BagActor->OnGrabbedActorPositionOverrideRequest.AddDynamic(BagOwner->PlayerInteraction, &UPlayerInteractionRaycast::SetGrabbedActorPositionOverride);
			BagActor->OnGrabbedActorPositionOverrideClearRequest.AddDynamic(BagOwner->PlayerInteraction, &UPlayerInteractionRaycast::ClearGrabbedActorPositionOverride);
			BagOwner->PlayerInteraction->OnPropGrabbed.AddDynamic(BagActor.Get(), &ABagActor::BeginItemGrab);
			BagOwner->PlayerInteraction->OnPropReleased.AddDynamic(BagActor.Get(), &ABagActor::CommitGrabbedItemAction);
			
			// Input events binding
			OnPlayerCursorHoverChanged.AddDynamic(BagActor.Get(), &ABagActor::OnPlayerCursorHoverChanged);
		}
		
		SetBagActorAttached(true);
		bPlayerCursorInBounds = false;
	}

	if (OnBagStateChanged.IsBound())
	{
		OnBagStateChanged.Broadcast(IsOpen());
	}
}

void UBagComponent::OnLeftRightInputAction(int Direction)
{
	if (OnPlayerLeftRightInput.IsBound())
	{
		OnPlayerLeftRightInput.Broadcast(Direction);
	}
}

int UBagComponent::GetBagSize() const
{
	return BagGridSize.X * BagGridSize.Y;
}

FVector2D UBagComponent::GetBagGridSize() const
{
	return BagGridSize;
}

int UBagComponent::GetFreeTileAmount()
{
	int FreeTileAmount = 0;
	for (FBagSlot& BagSlot: Items)
	{
		if (BagSlot.IsEmpty())
		{
			FreeTileAmount++;
		}
	}
	return FreeTileAmount;
}

bool UBagComponent::IsBagFull()
{
	return GetFreeTileAmount() == 0;
}

FBagSlot* UBagComponent::GetItemByPosition(const FVector2D& TilePos)
{
	return Items.FindByPredicate([=](const FBagSlot& BagSlot)
	{
		return BagSlot.TilePos == TilePos;
	});
}

FBagSlot* UBagComponent::GetItemByClass(const TSubclassOf<APickupItem> ItemClass){
	return Items.FindByPredicate([=](const FBagSlot& BagSlot)
	{
		return BagSlot.ItemClass == ItemClass;
	});
}

bool UBagComponent::HasItem(const TSubclassOf<APickupItem> ItemClass)
{
	if (GetItemByClass(ItemClass) != nullptr)
	{
		return true;
	}
	return false;
}

FBagSlot* UBagComponent::GetFirstFreeTile()
{
	return Items.FindByPredicate([=](const FBagSlot& BagSlot)
	{
		return BagSlot.ItemClass == nullptr;
	});
}

void UBagComponent::AddItem(TSubclassOf<APickupItem> ItemClass, int32 Amount)
{
	FBagSlot* BagSlot = GetItemByClass(ItemClass);
	if (BagSlot == nullptr) // New item
	{
		BagSlot = GetFirstFreeTile();
	}

	if (BagSlot == nullptr) // No free tiles
	{
		return;
	}

	BagSlot->ItemClass = ItemClass;
	BagSlot->Amount = BagSlot->Amount == 0 ? Amount : BagSlot->Amount + Amount;
	
	if (OnNewItemAdded.IsBound())
	{
		OnNewItemAdded.Broadcast(*BagSlot);
	}
}

void UBagComponent::RemoveItem(TSubclassOf<APickupItem> ItemClass, int32 Amount)
{
	FBagSlot* BagSlot = GetItemByClass(ItemClass);
	if (BagSlot == nullptr) return;

	if (BagSlot->Amount - Amount <= 0)
	{
		BagSlot->Clear();
	}
	else
	{
		BagSlot->ItemClass = ItemClass;
		BagSlot->Amount -= Amount;
	}
	
	if (OnItemRemoved.IsBound())
	{
		OnItemRemoved.Broadcast(*BagSlot);
	}
}
