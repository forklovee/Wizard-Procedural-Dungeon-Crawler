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

void UInventoryComponent::UpdateItemSlot(FInvSlot BagSlotOverride)
{
	if (BagSlotOverride.Index == -1) return;

	FInvSlot& InventoryBagSlot = Items[BagSlotOverride.Index];
	InventoryBagSlot.Amount = BagSlotOverride.Amount;
	InventoryBagSlot.ItemClass = BagSlotOverride.ItemClass;
}

void UInventoryComponent::AddItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	// FInvSlot* BagSlot = GetItemByClass(ItemClass);
	// if (BagSlot == nullptr) // New item
	// {
	// 	BagSlot = GetFirstFreeTile();
	// }
	//
	// if (BagSlot == nullptr) // No free tiles
	// {
	// 	return;
	// }
	//
	// BagSlot->ItemClass = ItemClass;
	// BagSlot->Amount = BagSlot->Amount == 0 ? Amount : BagSlot->Amount + Amount;
	//
	// if (OnNewItemAdded.IsBound())
	// {
	// 	OnNewItemAdded.Broadcast(*BagSlot);
	// }
}

void UInventoryComponent::RemoveItem(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	// FInvSlot* BagSlot = GetItemByClass(ItemClass);
	// if (BagSlot == nullptr) return;
	//
	// if (BagSlot->Amount - Amount <= 0)
	// {
	// 	BagSlot->Clear();
	// }
	// else
	// {
	// 	BagSlot->ItemClass = ItemClass;
	// 	BagSlot->Amount -= Amount;
	// }
	//
	// if (OnItemRemoved.IsBound())
	// {
	// 	OnItemRemoved.Broadcast(*BagSlot);
	// }
}
