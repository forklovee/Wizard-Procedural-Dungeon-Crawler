// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagUI.h"

#include "UI/Bag/BagItemTile.h"
#include "Components/Character/InventoryComponent.h"
#include "Items/Item.h"

#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"

TArray<UBagItemTile*>& UBagUI::Setup(UInventoryComponent* BagComponent)
{
	// GridSize = BagComponent->GetBagGridSize();
	// BagItemsPanel->ClearChildren();
	//
	// for (FBagSlot& BagSlot: BagComponent->GetItems())
	// {
	// 	UBagItemTile* BagItemTile = CreateNewBagTile(BagSlot);
	// 	BagItemTiles.Add(BagItemTile);
	// }

	return BagItemTiles;
}

TArray<AItem*> UBagUI::GetAllSpawnedActors() const
{
	TArray<AItem*> SpawnedActors;
	for (UWidget* Widget : BagItemsPanel->GetAllChildren())
	{
		if (const UBagItemTile* BagItemTile = Cast<UBagItemTile>(Widget))
		{
			if (AItem* PickupItem = BagItemTile->GetPickupItemActor())
			{
				SpawnedActors.Add(PickupItem);
			}
		}
	}
	return SpawnedActors;
}

UBagItemTile* UBagUI::CreateNewBagTile(FInvSlot& BagSlot)
{
	UBagItemTile* BagItemTile = CreateWidget<UBagItemTile>(GetWorld(), BagItemWidgetClass);
	BagItemTile->SetBagSlot(&BagSlot);

	const int ColIdx = BagSlot.Index % (int)GridSize.X;
	const int RowIdx = BagSlot.Index / (GridSize.Y+1.0f);

	BagItemsPanel->AddChildToUniformGrid(BagItemTile, RowIdx, ColIdx);		
	BagItemTile->UpdateVisualData();

	UE_LOG(LogTemp, Display, TEXT("%i Added: %i, %i"), BagItemsPanel->GetChildrenCount(), ColIdx, RowIdx);
	
	// if (OnNewItemTileCreated.IsBound())
	// {
	// 	OnNewItemTileCreated.Broadcast(BagItemTile);
	// }
	
	return BagItemTile;
}
