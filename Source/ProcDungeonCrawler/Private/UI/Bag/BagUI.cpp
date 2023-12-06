// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagUI.h"

#include "Characters/Player/BagActor.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Character/BagComponent.h"
#include "Items/PickupItem.h"
#include "UI/Bag/BagItemTile.h"

TArray<UBagItemTile*>& UBagUI::Setup(ABagActor* BagActor, UBagComponent* BagComponent)
{
	GridSize = BagComponent->GetBagGridSize();
	BagItemsPanel->ClearChildren();

	for (FBagSlot& BagSlot: BagComponent->GetItems())
	{
		UBagItemTile* BagItemTile = CreateNewBagTile(BagSlot);
		BagItemTiles.Add(BagItemTile);
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

UBagItemTile* UBagUI::CreateNewBagTile(FBagSlot& BagSlot)
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
