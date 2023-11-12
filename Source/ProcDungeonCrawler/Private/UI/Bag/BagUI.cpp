// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagUI.h"

#include "Characters/Player/BagActor.h"
#include "Components/PanelWidget.h"
#include "Items/PickupItem.h"
#include "UI/Bag/BagItemTile.h"

TMap<TSubclassOf<APickupItem>, UBagItemTile*> UBagUI::SetupBagUI(ABagActor* BagActor, TMap<TSubclassOf<APickupItem>, int32>* PawnItemsPtr)
{
	TMap<TSubclassOf<APickupItem>, UBagItemTile*> BagItemTiles;
	
	TArray<TSubclassOf<APickupItem>> ItemClasses;
	if (PawnItemsPtr != nullptr)
	{
		PawnItemsPtr->GetKeys(ItemClasses);
	}
	for (const TSubclassOf<APickupItem>& ItemClass : ItemClasses)
	{
		const int* ItemAmountPtr = PawnItemsPtr->Find(ItemClass);
		if (ItemAmountPtr == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to add new item tile: Item amount not found!"))
			continue;
		}
		UBagItemTile* BagItemTile = CreateNewItemTile(ItemClass, *ItemAmountPtr);
		BagItemTiles.Add(ItemClass, BagItemTile);
	}

	return BagItemTiles;
}

TArray<APickupItem*> UBagUI::GetAllSpawnedActors() const
{
	TArray<APickupItem*> SpawnedActors;
	for (UWidget* Widget : BagItemsPanel->GetAllChildren())
	{
		if (const UBagItemTile* BagItemTile = Cast<UBagItemTile>(Widget))
		{
			if (APickupItem* PickupItem = BagItemTile->GetPickupItemActor())
			{
				SpawnedActors.Add(PickupItem);
			}
		}
	}
	return SpawnedActors;
}

void UBagUI::ChangePage(int Direction)
{
	// Destroy all visible items
	if (CurrentPage + Direction != CurrentPage)
	{
		const int StartIdx = CurrentPage * 3;
		for (int Idx = StartIdx; Idx < StartIdx+3; Idx++)
		{
			UBagItemTile* BagItemTile = Cast<UBagItemTile>(BagItemsPanel->GetChildAt(Idx));
			if (BagItemTile == nullptr)
			{
				continue;
			}
			BagItemTile->DestroyPickupItem();
		}
	}
	
	const int MaxPage = FMath::CeilToInt32(BagItemsPanel->GetChildrenCount() / 3.0);
	CurrentPage = FMath::Clamp(CurrentPage + Direction, 0, MaxPage - 1);

	// Spawn new items from page
	const int StartIdx = CurrentPage * 3;
	for (int Idx = StartIdx; Idx < StartIdx+3; Idx++)
	{
		UBagItemTile* BagItemTile = Cast<UBagItemTile>(BagItemsPanel->GetChildAt(Idx));
		if (BagItemTile == nullptr || BagItemTile->GetPickupItemClass() == nullptr)
		{
			continue;
		}
		BagItemTile->SpawnPickupItem();
	}
}

UBagItemTile* UBagUI::CreateNewItemTile(TSubclassOf<APickupItem> ItemClass, int32 Amount)
{
	UBagItemTile* BagItemTile = CreateWidget<UBagItemTile>(GetWorld(), BagItemWidgetClass);
	BagItemTile->SetPickupItemClass(ItemClass);
	BagItemTile->SetItemAmountTextBlock(Amount);
	BagItemsPanel->AddChild(BagItemTile);

	if (OnNewItemTileCreated.IsBound())
	{
		OnNewItemTileCreated.Broadcast(BagItemTile);
	}
	
	return BagItemTile;
}
