// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagItemTile.h"
#include "Items/PickupItem.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBagItemTile::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBagItemTile::SetCanSetNewItem(bool State)
{
	// If could set new item, remove the hover event bindings
	if (bCanSetNewItem)
	{
		TileButton->OnHovered.RemoveDynamic(this, &UBagItemTile::OnTileHovered);
		TileButton->OnUnhovered.RemoveDynamic(this, &UBagItemTile::OnTileUnHovered);
	}
	
	bCanSetNewItem = State;
	if (bCanSetNewItem)
	{
		TileButton->OnHovered.AddDynamic(this, &UBagItemTile::OnTileHovered);
		TileButton->OnUnhovered.AddDynamic(this, &UBagItemTile::OnTileUnHovered);
	}
}

void UBagItemTile::SpawnPickupItem()
{
	if (OnSpawnPickupItemRequest.IsBound())
	{
		OnSpawnPickupItemRequest.Broadcast(this);
	}
}

void UBagItemTile::DestroyPickupItem()
{
	if (OnDestroyPickupItemRequest.IsBound())
	{
		OnDestroyPickupItemRequest.Broadcast(this);
	}
}

void UBagItemTile::SetPickupItemClass(TSubclassOf<APickupItem> PickupItem)
{
	bIsTargeted = false;
	PickupItemClass = PickupItem;

	if (PickupItemClass == nullptr)
	{
		RemoveFromParent();
	}
}

TSubclassOf<APickupItem> UBagItemTile::GetPickupItemClass() const
{
	return PickupItemClass;
}

void UBagItemTile::SetPickupItemActor(APickupItem* PickupItem)
{
	// if (PickupItemActor != nullptr)
	// {
		// PickupItemActor->OnItemGrabbedChanged.RemoveDynamic(this, &UBagItemTile::OnItemGrabbed);
	// }
	PickupItemActor = PickupItem;
	// if (PickupItem != nullptr)
	// {
	// 	PickupItem->OnItemGrabbedChanged.AddDynamic(this, &UBagItemTile::OnItemGrabbed);
	// }
}

APickupItem* UBagItemTile::GetPickupItemActor() const
{
	return PickupItemActor.Get();
}

void UBagItemTile::SetItemAmountTextBlock(int32 ItemAmount)
{
	ItemAmountTextBlock->SetText(FText::FromString(FString::FromInt(ItemAmount)));
}

int32 UBagItemTile::GetIndex() const
{
	return GetParent()->GetChildIndex(this);
}

bool UBagItemTile::IsTargeted() const
{
	return bIsTargeted;
}

void UBagItemTile::OnTileHovered()
{
	bIsTargeted = true;
}

void UBagItemTile::OnTileUnHovered()
{
	bIsTargeted = false;
}