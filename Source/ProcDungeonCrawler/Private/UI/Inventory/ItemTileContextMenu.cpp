// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemTileContextMenu.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "UI/Inventory/ContextMenuActionButton.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "UI/Inventory/ItemTile.h"

void UItemTileContextMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(ItemNameText))
		ItemNameText->SetText(FText::FromString("Item Name"));
	
	if (IsValid(UseButton))
		UseButton->Button->OnClicked.AddDynamic(this, &UItemTileContextMenu::OnUseActionButton);
	if (IsValid(DropButton))
		DropButton->Button->OnClicked.AddDynamic(this, &UItemTileContextMenu::OnDropActionButton);
	if (IsValid(InspectButton))
		InspectButton->Button->OnClicked.AddDynamic(this, &UItemTileContextMenu::OnInspectActionButton);
	if (IsValid(CloseButton))
		CloseButton->Button->OnClicked.AddDynamic(this, &UItemTileContextMenu::OnCloseActionButton);

	SetVisibility(ESlateVisibility::Collapsed);
}

void UItemTileContextMenu::OnItemTileUpdated(UItemTile* ItemTile, FInventorySlot& InventorySlot)
{
	if (ItemTile == TargetItemTile)
	{
		if (InventorySlot.Amount <= 0)
		{
			Close();
		}
		else
		{
			SetItemNameText(InventorySlot);
		}
	}
}

void UItemTileContextMenu::OnUseActionButton()
{
	if (OnUseActionButtonClicked.IsBound())
	{
		OnUseActionButtonClicked.Broadcast();
	}
}

void UItemTileContextMenu::OnDropActionButton()
{
	if (OnDropActionButtonClicked.IsBound())
	{
		OnDropActionButtonClicked.Broadcast();
	}
}

void UItemTileContextMenu::OnInspectActionButton()
{
	if (OnInspectActionButtonClicked.IsBound())
	{
		OnInspectActionButtonClicked.Broadcast();
	}
}

void UItemTileContextMenu::OnCloseActionButton()
{
	Close();
	if (OnCancelActionButtonClicked.IsBound())
	{
		OnCancelActionButtonClicked.Broadcast();
	}
}

void UItemTileContextMenu::Open(UItemTile* ItemTile, FVector2D AbsolutePosition)
{
	TargetItemTile = ItemTile;
	if (TargetItemTile.IsValid())
	{
		OnUseActionButtonClicked.AddDynamic(TargetItemTile.Get(), &UItemTile::UseItem);
		OnDropActionButtonClicked.AddDynamic(TargetItemTile.Get(), &UItemTile::DropItem);
		OnInspectActionButtonClicked.AddDynamic(TargetItemTile.Get(), &UItemTile::InspectItem);
	}
	else
	{
		return;
	}

	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(
		GetWorld(),
		AbsolutePosition,
		PixelPosition,
		ViewportPosition
		);

	if (UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this))
	{
		CanvasPanelSlot->SetPosition(ViewportPosition);
	}
	
	const FInventorySlot& InventorySlot = ItemTile->GetItemData();
	ItemTile->OnItemTileUpdated.AddDynamic(this, &UItemTileContextMenu::OnItemTileUpdated);
	if (TargetItemTile.IsValid())
	{
		SetItemNameText(InventorySlot);
	}
	SetVisibility(ESlateVisibility::Visible);

	if (OnItemTileContextMenuOpened.IsBound())
	{
		OnItemTileContextMenuOpened.Broadcast();
	}
}

void UItemTileContextMenu::Close()
{
	if (TargetItemTile.IsValid())
	{
		TargetItemTile->OnItemTileUpdated.RemoveDynamic(this, &UItemTileContextMenu::OnItemTileUpdated);
		OnUseActionButtonClicked.RemoveDynamic(TargetItemTile.Get(), &UItemTile::UseItem);
		OnDropActionButtonClicked.RemoveDynamic(TargetItemTile.Get(), &UItemTile::DropItem);
		OnInspectActionButtonClicked.RemoveDynamic(TargetItemTile.Get(), &UItemTile::InspectItem);
	}
	
	TargetItemTile = nullptr;
	SetVisibility(ESlateVisibility::Collapsed);

	if (OnItemTileContextMenuClosed.IsBound())
	{
		OnItemTileContextMenuClosed.Broadcast();
	}
}

void UItemTileContextMenu::OnInventoryVisibilityChanged(bool bNewIsInventoryVisible)
{
	if (!bNewIsInventoryVisible)
	{
		Close();
	}
}

void UItemTileContextMenu::SetItemNameText(const FInventorySlot& InventorySlot) const
{
	const FText ItemName = FText::Format(FText::FromString("{0} ({1})"), InventorySlot.ItemNameText, FText::AsNumber(InventorySlot.Amount));
	ItemNameText->SetText(ItemName);
}

