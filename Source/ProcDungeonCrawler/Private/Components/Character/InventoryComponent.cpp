// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/InventoryComponent.h"
#include "Items/Item.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<FInventorySlot>& UInventoryComponent::GetItems()
{
	return Items;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::AddItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	FInventorySlot* InventorySlot = Items.FindByPredicate([=](FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});

	if (InventorySlot == nullptr)
	{
		InventorySlot = &Items[Items.Add(FInventorySlot(ItemClass, Amount))];
		
		bool bFoundFreeTile = false;
		for (int X = 0; X < InventorySize.X; X++)
		{
			for (int Y = 0; Y < InventorySize.Y; Y++)
			{
				if (IsTileFree(FVector2D(X, Y)))
				{
					bFoundFreeTile = true;
					InventorySlot->TilePos = FVector2D(X, Y);
					break;
				}
			}

			if (bFoundFreeTile)
			{
				break;
			}
		}
	}
	else
	{
		InventorySlot->Amount += Amount;
	}
	
	if (OnItemAdded.IsBound())
	{
		OnItemAdded.Broadcast(*InventorySlot);
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	int SlotIndex = 0;
	FInventorySlot* InventorySlot = nullptr;
	for (FInventorySlot& Slot: Items)
	{
		if (Slot.ItemClass == ItemClass)
		{
			InventorySlot = &Slot;
			break;
		}
		SlotIndex++;
	}
	
	if (InventorySlot == nullptr)
	{
		return;
	}
	
	InventorySlot->Amount -= Amount;
	if (InventorySlot->Amount <= 0)
	{
		Items.RemoveAt(SlotIndex);
	}

	if (OnItemRemoved.IsBound())
	{
		OnItemRemoved.Broadcast(*InventorySlot);
	}
}

bool UInventoryComponent::HasItem(TSubclassOf<AItem> ItemClass)
{
	return Items.FindByPredicate([=](FInventorySlot& Slot) {
           		return Slot.ItemClass == ItemClass;
           	}) != nullptr;
}

int UInventoryComponent::GetItemAmount(TSubclassOf<AItem> ItemClass)
{
	const FInventorySlot* InventorySlot = Items.FindByPredicate([=](FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});
	
	if (InventorySlot != nullptr)
	{
		return InventorySlot->Amount;
	}
	
	return 0;
}

bool UInventoryComponent::IsTileFree(FVector2D TilePos)
{
	for (FInventorySlot& InventorySlot: Items)
	{
		if (InventorySlot.TilePos == TilePos)
		{
			return false;
		}
	}
	return true;
}
