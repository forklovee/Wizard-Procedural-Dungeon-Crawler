﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemTile.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Character/InventoryComponent.h"

void UItemTile::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button != nullptr)
	{
		Button->OnPressed.AddDynamic(this, &UItemTile::UseItem);
	}
}

 void UItemTile::NativeConstruct()
{
	Super::NativeConstruct();

 	
}

void UItemTile::UpdateData(FInventorySlot InventorySlot)
{
 	ItemData = InventorySlot;
	
 	AmountTextBlock->SetText(FText::FromString(FString::FromInt(InventorySlot.Amount)));
	ItemImage->SetBrushFromTexture(nullptr);
}

void UItemTile::UseItem()
{
	if (OnUseItemRequest.IsBound())
	{
		UE_LOG(LogTemp, Display, TEXT("Use item class: %s, amount: %d"), *ItemData.ItemClass->GetName(), ItemData.Amount);
		OnUseItemRequest.Broadcast(ItemData.ItemClass, ItemData.Amount);
	}
}

void UItemTile::InspectItem()
{
	if (OnInspectItemRequest.IsBound())
	{
		OnInspectItemRequest.Broadcast(ItemData.ItemClass);
	}
}

void UItemTile::DropItem()
{
	if (OnDropItemRequest.IsBound())
	{
		OnDropItemRequest.Broadcast(ItemData.ItemClass, ItemData.Amount);
	}
}
