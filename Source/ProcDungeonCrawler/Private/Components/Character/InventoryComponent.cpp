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
}

FVector2D UInventoryComponent::GetFirstFreeTile() const
{
	for (int X = 0; X < InventorySize.X; X++)
	{
		for (int Y = 0; Y < InventorySize.Y; Y++)
		{
			if (ItemSlots.FindByPredicate([=](const FInventorySlot& Slot) {
				return Slot.TilePos == FVector2D(X, Y);
			}) == nullptr)
			{
				return FVector2D(X, Y);
			}
		}
	}
	return FVector2D(-1, -1);
}

void UInventoryComponent::AddItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	int SlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});
	
	UE_LOG(LogTemp, Display, TEXT("aaaa"))

	if (SlotIndex != -1) // Item exists, add amount
	{
		ItemSlots[SlotIndex].Amount += Amount;
		UE_LOG(LogTemp, Display, TEXT("Item: %s - Amount: %d"), *ItemClass->GetName(), ItemSlots[SlotIndex].Amount)

		if (OnItemAdded.IsBound())
		{
			OnItemAdded.Broadcast(SlotIndex, ItemSlots[SlotIndex]);
		}
		return;
	}
	// Item doesn't exist, add new item
	// Check if there is a free tile
	const FVector2D FreeTilePosition = GetFirstFreeTile();
	if (FreeTilePosition == FVector2D(-1, -1))
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory full"))
		return;
	}

	SlotIndex = ItemSlots.Add(FInventorySlot(ItemClass, Amount));
	ItemSlots[SlotIndex].TilePos = FreeTilePosition;

	UE_LOG(LogTemp, Display, TEXT("Item: %s - Amount: %d"), *ItemClass->GetName(), ItemSlots[SlotIndex].Amount)
	
	if (OnItemAdded.IsBound())
	{
		OnItemAdded.Broadcast(SlotIndex, ItemSlots[SlotIndex]);
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	int SlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
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
