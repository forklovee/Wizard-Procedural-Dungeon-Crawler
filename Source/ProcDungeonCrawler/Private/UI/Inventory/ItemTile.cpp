// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemTile.h"

#include "Channels/MovieSceneChannel.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/Character/InventoryComponent.h"
#include "Materials/MaterialInstanceConstant.h"

void UItemTile::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (Button != nullptr)
	{
		Button->OnPressed.AddDynamic(this, &UItemTile::DoubleClickUseItem);
		
		// Button->OnHovered.AddDynamic(this, &UItemTile::OnItemTileHovered);
		// Button->OnUnhovered.AddDynamic(this, &UItemTile::OnItemTileUnHovered);
	}
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

void UItemTile::UpdateVisualData()
{
	if (ItemDataPtr->ItemClass == nullptr)
	{
		Button->SetVisibility(ESlateVisibility::Collapsed);
		
		AmountTextBlock->SetVisibility(ESlateVisibility::Collapsed);

		EquippedImage->SetVisibility(ESlateVisibility::Collapsed);
		ItemImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Button->SetVisibility(ESlateVisibility::Visible);
		
		AmountTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		AmountTextBlock->SetText(FText::FromString(FString::FromInt(ItemDataPtr->Amount)));

		EquippedImage->SetVisibility(ItemDataPtr->SpawnedActor.IsValid() ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		
		ItemImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(ItemTileMaterial, this))
		{
			Material->SetTextureParameterValue(FName("ItemTexture"), ItemDataPtr->ItemIcon.Get());
			ItemImage->SetBrushFromMaterial(Material);
		}
	}

	if (OnItemTileUpdated.IsBound())
	{
		OnItemTileUpdated.Broadcast(this, *ItemDataPtr);
	}
}

void UItemTile::SetInventorySlot(FInventorySlot& InventorySlot)
{
 	ItemDataPtr = &InventorySlot;
	UpdateVisualData();
}

void UItemTile::DoubleClickUseItem()
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
		OnUseItemRequest.Broadcast(*ItemDataPtr);
	}
	UpdateVisualData();
}

void UItemTile::UseItem()
{
	if (OnUseItemRequest.IsBound())
	{
		OnUseItemRequest.Broadcast(*ItemDataPtr);
	}
	UpdateVisualData();
}

void UItemTile::InspectItem()
{
	if (OnInspectItemRequest.IsBound())
	{
		OnInspectItemRequest.Broadcast(*ItemDataPtr);
	}
	UpdateVisualData();
}

void UItemTile::DropItem()
{
	if (OnDropItemRequest.IsBound())
	{
		OnDropItemRequest.Broadcast(*ItemDataPtr);
	}
	UpdateVisualData();
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
