// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/BagItemTile.h"
#include "Items/PickupItem.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Character/BagComponent.h"

void UBagItemTile::NativeConstruct()
{
	Super::NativeConstruct();

	DefaultButtonBrush = TileButton->GetStyle().Normal;
	HoverButtonBrush = TileButton->GetStyle().Hovered;
}

void UBagItemTile::UpdateVisualData()
{
	if (BagSlot == nullptr) return;

	DragItemTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	SetItemAmountTextBlock(BagSlot->Amount);
}

void UBagItemTile::SetCanBeTargeted(bool bNewCanBeTargeted)
{
	UE_LOG(LogTemp, Display, TEXT("Set %s can be targeted"), bCanBeTargeted ? TEXT("can't") : TEXT("can"))
	
	// Update button style
	FButtonStyle TileButtonStyle = TileButton->GetStyle();
	TileButtonStyle.Hovered = bCanBeTargeted ? HoverButtonBrush : DefaultButtonBrush;
	TileButton->SetStyle(TileButtonStyle);

	// If no change in targetable state, return
	if (bCanBeTargeted == bNewCanBeTargeted) return;
	bCanBeTargeted = bNewCanBeTargeted;

	// Register hover delegates
	if (bCanBeTargeted)
	{
		TileButton->OnHovered.AddDynamic(this, &UBagItemTile::OnTileHovered);
		TileButton->OnUnhovered.AddDynamic(this, &UBagItemTile::OnTileUnHovered);
	}
	else
	{
		TileButton->OnHovered.RemoveDynamic(this, &UBagItemTile::OnTileHovered);
		TileButton->OnUnhovered.RemoveDynamic(this, &UBagItemTile::OnTileUnHovered);
	}
}

void UBagItemTile::SetBagSlot(FBagSlot* NewBagSlot)
{
	BagSlot = NewBagSlot;
}

FBagSlot* UBagItemTile::GetBagSlot() const
{
	return BagSlot;
}

void UBagItemTile::SetPickupItemActor(APickupItem* PickupItem)
{
	PickupItemActor = PickupItem;
}

APickupItem* UBagItemTile::GetPickupItemActor() const
{
	return PickupItemActor.Get();
}

void UBagItemTile::SetItemAmountTextBlock(int32 ItemAmount)
{
	ItemAmountTextBlock->SetText(FText::FromString(FString::FromInt(ItemAmount)));
}

void UBagItemTile::OnTileHovered()
{
	if (OnHoverChanged.IsBound())
	{
		OnHoverChanged.Broadcast(this, true);
	}
}

void UBagItemTile::OnTileUnHovered()
{
	if (OnHoverChanged.IsBound())
	{
		OnHoverChanged.Broadcast(this, false);
	}
}