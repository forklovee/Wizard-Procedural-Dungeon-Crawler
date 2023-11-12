// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagItemTile.h"
#include "Items/PickupItem.h"

#include "Components/Button.h"

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
	if (PickupItemActor != nullptr)
	{
		PickupItemActor->OnItemGrabbedChanged.RemoveDynamic(this, &UBagItemTile::OnItemGrabbed);
	}
	PickupItemActor = PickupItem;
	if (PickupItem != nullptr)
	{
		PickupItem->OnItemGrabbedChanged.AddDynamic(this, &UBagItemTile::OnItemGrabbed);
	}
}

APickupItem* UBagItemTile::GetPickupItemActor() const
{
	return PickupItemActor.Get();
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

void UBagItemTile::OnItemGrabbed(APickupItem* Item, bool bIsGrabbed)
{
	if (OnItemActorGrabbedChanged.IsBound())
	{
		OnItemActorGrabbedChanged.Broadcast(this, bIsGrabbed);
	}
}

void UBagItemTile::NativeConstruct()
{
	Super::NativeConstruct();

	if (TileButton != nullptr)
	{
		TileButton->OnHovered.AddDynamic(this, &UBagItemTile::OnTileHovered);
		TileButton->OnUnhovered.AddDynamic(this, &UBagItemTile::OnTileUnHovered);
	}
}
