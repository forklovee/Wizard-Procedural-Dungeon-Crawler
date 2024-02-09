// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/InventoryComponent.h"
#include "Items/Item.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < InventorySize.X; i++)
	{
		for (int j = 0; j < InventorySize.Y; j++)
		{
			ItemSlots.Add(FInventorySlot(i, j));
		}
	}
}

int UInventoryComponent::GetFirstFreeInventorySlotId() const
{
	return ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.IsEmpty();
	});
}

void UInventoryComponent::AddItem(AItem* ItemActor, int32 Amount)
{
	if (!IsValid(ItemActor))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Item actor."))
		return;
	}
	
	const TSubclassOf<AItem> ItemClass = ItemActor->GetClass();
	int SlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});
	
	if (SlotIndex != -1) // Item exists, add amount
	{
		ItemSlots[SlotIndex].Amount += Amount;

		if (OnItemAdded.IsBound())
		{
			OnItemAdded.Broadcast(SlotIndex, ItemSlots[SlotIndex]);
		}
		ItemActor->Destroy();
		return;
	}
	// Item doesn't exist, add new item
	// Check if there is a free tile
	SlotIndex = GetFirstFreeInventorySlotId();
	if (SlotIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory full"))
		return;
	}

	ItemSlots[SlotIndex].ItemNameText = ItemActor->GetItemName();
	ItemSlots[SlotIndex].ItemClass = ItemClass;
	ItemSlots[SlotIndex].Amount = Amount;
	ItemSlots[SlotIndex].ItemIcon = ItemActor->GetItemIcon();
	
	UE_LOG(LogTemp, Display, TEXT("Item: %s - Amount: %d"), *ItemClass->GetName(), ItemSlots[SlotIndex].Amount)
	
	if (OnItemAdded.IsBound())
	{
		OnItemAdded.Broadcast(SlotIndex, ItemSlots[SlotIndex]);
	}

	ItemActor->Destroy();
}

void UInventoryComponent::RemoveItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	const int SlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});
	
	if (SlotIndex == -1)
	{
		UE_LOG(LogTemp, Display, TEXT("Item not found."))
		return;
	}

	FInventorySlot* InventorySlot = &ItemSlots[SlotIndex];
	InventorySlot->Amount -= Amount;
	if (InventorySlot->Amount <= 0)
	{
		InventorySlot->Clear();
	}

	if (OnItemRemoved.IsBound())
	{
		OnItemRemoved.Broadcast(SlotIndex, *InventorySlot);
	}
}

bool UInventoryComponent::HasItem(TSubclassOf<AItem> ItemClass)
{
	return ItemSlots.FindByPredicate([=](FInventorySlot& Slot) {
           		return Slot.ItemClass == ItemClass;
           	}) != nullptr;
}

int UInventoryComponent::GetItemAmount(TSubclassOf<AItem> ItemClass)
{
	const FInventorySlot* InventorySlot = ItemSlots.FindByPredicate([=](FInventorySlot& Slot) {
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
	for (FInventorySlot& InventorySlot: ItemSlots)
	{
		if (InventorySlot.TilePos == TilePos)
		{
			return false;
		}
	}
	return true;
}
