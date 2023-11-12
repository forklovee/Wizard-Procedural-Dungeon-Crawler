// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagUI.h"

#include "Characters/Player/BagActor.h"
#include "Components/PanelWidget.h"
#include "Items/PickupItem.h"
#include "UI/Bag/BagItemTile.h"

void UBagUI::SetupBagUI(ABagActor* BagActor, TArray<TSubclassOf<APickupItem>>& ItemClasses)
{
	for (const TSubclassOf<APickupItem>& ItemClass : ItemClasses)
	{
		UBagItemTile* BagItemTile = CreateWidget<UBagItemTile>(GetWorld(), BagItemWidgetClass);
		BagItemTile->SetPickupItemClass(ItemClass);
		BagItemsPanel->AddChild(BagItemTile);
		if (BagActor == nullptr)
		{
			continue;
		}
		BagItemTile->OnSpawnPickupItemRequest.AddDynamic(BagActor, &ABagActor::SpawnItemActor);
		BagItemTile->OnDestroyPickupItemRequest.AddDynamic(BagActor, &ABagActor::DestroyItemActor);
	}
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
		if (BagItemTile == nullptr)
		{
			continue;
		}
		BagItemTile->SpawnPickupItem();
	}
}
