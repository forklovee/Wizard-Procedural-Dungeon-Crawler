// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemTile.h"

#include "Channels/MovieSceneChannel.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/Character/InventoryComponent.h"

void UItemTile::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (Button != nullptr)
	{
		Button->OnPressed.AddDynamic(this, &UItemTile::UseItem);
		
		// Button->OnHovered.AddDynamic(this, &UItemTile::OnItemTileHovered);
		// Button->OnUnhovered.AddDynamic(this, &UItemTile::OnItemTileUnHovered);
	}

	ContextMenu->SetVisibility(ESlateVisibility::Collapsed);
}

 void UItemTile::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UItemTile::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UE_LOG(LogTemp, Display, TEXT("%f, %f"), MyGeometry.AbsolutePosition.X, MyGeometry.AbsolutePosition.Y);
	// Right click context menu
	if (OnContextMenuRequested.IsBound())
	{
		OnContextMenuRequested.Broadcast(this, MouseEvent.GetScreenSpacePosition());
	}
	
	return FReply::Handled();
}

void UItemTile::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (OnItemTileMouseHovered.IsBound())
	{
		OnItemTileMouseHovered.Broadcast(this);
	}
}

void UItemTile::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (OnItemTileMouseUnHovered.IsBound())
	{
		OnItemTileMouseUnHovered.Broadcast(this);
	}
}

void UItemTile::UpdateData(FInventorySlot InventorySlot)
{
 	ItemData = InventorySlot;

	if (InventorySlot.ItemClass == nullptr)
	{
		Button->SetVisibility(ESlateVisibility::Collapsed);
		
		AmountTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		
		ItemImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Button->SetVisibility(ESlateVisibility::Visible);
		
		AmountTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		AmountTextBlock->SetText(FText::FromString(FString::FromInt(InventorySlot.Amount)));
		
		ItemImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UItemTile::UseItem()
{
	if (UseItemTapCount > 2) return;
	
	UseItemTapCount++;
	switch (UseItemTapCount)
	{
		default:
		case 1:
			GetWorld()->GetTimerManager().SetTimer(UseItemDoubleTapTimerHandle, this, &UItemTile::ResetUseItemTapCounter, 0.5f, false);
			return;
		case 2:
			GetWorld()->GetTimerManager().ClearTimer(UseItemDoubleTapTimerHandle);
			UseItemTapCount = 0;
			break;
	}

	if (OnUseItemRequest.IsBound())
	{
		OnUseItemRequest.Broadcast(ItemData.ItemClass);
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

void UItemTile::ResetUseItemTapCounter()
{
	UseItemTapCount = 0;
}

void UItemTile::OnItemTileHovered()
{
	// if (OnItemTileMouseHovered.IsBound())
	// {
	// 	OnItemTileMouseHovered.Broadcast(this);
	// }
}

void UItemTile::OnItemTileUnHovered()
{
	// if (OnItemTileMouseUnHovered.IsBound())
	// {
	// 	OnItemTileMouseUnHovered.Broadcast(this);
	// }
}
