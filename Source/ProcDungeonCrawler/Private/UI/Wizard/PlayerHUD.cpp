// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\UI\Wizard\PlayerHUD.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/Human/Player/PlayerPawn.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/Wizard/RuneCastSlot.h"
#include "Components/ProgressBar.h"
#include "Components/UniformGridPanel.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/PawnStats.h"
#include "UI/Inventory/ItemTile.h"
#include "UI/Inventory/ItemTileContextMenu.h"

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPlayerHUD::UpdateInventoryData()
{
	// for (FInventorySlot& InventorySlot : Player->Inventory->GetItems())
	// {
	// 	if (InventorySlot.ItemClass == nullptr) continue;
	// 	
	// 	UItemTile* ItemTile = GetItemTileAtPosition(InventorySlot.TilePos);
	// 	ItemTile->UpdateData(InventorySlot);
	// }
}

UItemTile* UPlayerHUD::GetItemTileAtPosition(FVector2D TilePos) const
{
	const int TileIdx = TilePos.X + (TilePos.Y * InventorySize.X);
	if (TileIdx < 0 || TileIdx > ItemTiles.Num()-1) return nullptr;

	return ItemTiles[TileIdx];
}

void UPlayerHUD::SetupInventory(APlayerPawn* PlayerPawn)
{
	InventoryGrid->ClearChildren();
	
	if (PlayerPawn == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("PlayerPawn is null"))
		return;
	}
	Player = PlayerPawn;

	OnInventoryVisibilityChanged.AddDynamic(InventoryContextMenu, &UItemTileContextMenu::OnInventoryVisibilityChanged);
	InventoryContextMenu->OnItemTileContextMenuOpened.AddDynamic(this, &UPlayerHUD::DisableAllItemTiles);
	InventoryContextMenu->OnItemTileContextMenuClosed.AddDynamic(this, &UPlayerHUD::EnableAllItemTiles);
	
	InventorySize = Player->Inventory->GetInventorySize();
	const int TargetSlotAmount = static_cast<int>(InventorySize.X * InventorySize.Y);
	if (TargetSlotAmount == ItemTiles.Num())
		return;
	
	for (int Y = 0; Y < static_cast<int>(InventorySize.Y); Y++)
	{
		for (int X = 0; X < static_cast<int>(InventorySize.X); X++)
		{
			UItemTile* ItemTile = CreateWidget<UItemTile>(GetWorld(), ItemTileClass);
			ItemTile->OnUseItemRequest.AddDynamic(PlayerPawn, &APlayerPawn::UseItem);
			// ItemTile->OnDropItemRequest.AddDynamic(PlayerPawn, &APlayerPawn::DropItem);
			
			ItemTile->OnContextMenuRequested.AddDynamic(InventoryContextMenu, &UItemTileContextMenu::Open);
			ItemTile->OnItemTileMouseHovered.AddDynamic(this, &UPlayerHUD::ChangeFocusedItemTile);
			ItemTile->OnItemTileMouseUnHovered.AddDynamic(this, &UPlayerHUD::ClearFocusedItemTile);
			
			ItemTiles.Add(ItemTile);
			InventoryGrid->AddChildToUniformGrid(ItemTile, X, Y);
			ItemTile->UpdateData(FInventorySlot(X, Y));
		}
	}
}

void UPlayerHUD::ToggleInventoryVisibility()
{
	SetInventoryVisible(!bInventoryIsVisible);
}

void UPlayerHUD::SetInventoryVisible(bool bNewIsVisible)
{
	bInventoryIsVisible = bNewIsVisible;
	
	InventoryCanvas->SetVisibility(!bInventoryIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	// HudCanvas->SetVisibility(bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	
	if (OnInventoryVisibilityChanged.IsBound())
	{
		OnInventoryVisibilityChanged.Broadcast(bInventoryIsVisible);
	}

	if (!bInventoryIsVisible)
	{
		return;
	}
}

void UPlayerHUD::UpdateInventorySlot(int SlotIndex, FInventorySlot InventorySlot)
{
	ItemTiles[SlotIndex]->UpdateData(InventorySlot);
}

void UPlayerHUD::ToggleContextMenuForFocusedItemTile()
{
	UE_LOG(LogTemp, Display, TEXT("Hovered!!!!!! %i"), bCanContextMenuOpen)

	
}

void UPlayerHUD::ChangeFocusedItemTile(UItemTile* NewFocusedItemTile)
{
	UE_LOG(LogTemp, Display, TEXT("%s hovered"), *NewFocusedItemTile->GetName())
	
	if (bCanContextMenuOpen) return;
	FocusedItemTile = NewFocusedItemTile;
}

void UPlayerHUD::ClearFocusedItemTile(UItemTile* NewUnFocusedItemTile)
{
	UE_LOG(LogTemp, Display, TEXT("%s not hovered"), *NewUnFocusedItemTile->GetName())
	
	if (bCanContextMenuOpen) return;
	FocusedItemTile = nullptr;
}

void UPlayerHUD::EnableAllItemTiles()
{
	for (UItemTile* ItemTile : ItemTiles)
	{
		ItemTile->SetIsEnabled(true);
	}
}

void UPlayerHUD::DisableAllItemTiles()
{
	for (UItemTile* ItemTile : ItemTiles)
	{
		ItemTile->SetIsEnabled(false);
	}
}

void UPlayerHUD::OnNewInteractionTarget(FText ActorName, FName InteractionType, bool bCanBeGrabbed)
{
	if (OnInteractionTargetChanged.IsBound())
	{
		OnInteractionTargetChanged.Broadcast(ActorName, InteractionType, bCanBeGrabbed);
	}
}

void UPlayerHUD::OnRuneCasted(int RuneIdx, URune* RuneCast, TArray<URune*>& CastedRunes)
{
	// if (RuneSlots == nullptr || RuneIdx < 0 || RuneIdx > RuneCastSlots.Num()-1) return;
	//
	// URuneCastSlot* RuneCastSlot = RuneCastSlots[RuneIdx];
	// RuneCastSlot->UseRune();
	//
	// CastedRuneHistory->UpdateRuneCastHistory(CastedRunes);
}

bool UPlayerHUD::IsSlotEmpty(int SlotIdx) const
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return false;
	
	URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx];
	return RuneCastSlot->IsEmpty();
}

void UPlayerHUD::UnbindRuneToSlot(int SlotIdx)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;

	RuneCastSlots[SlotIdx]->ClearRuneData();
}

void UPlayerHUD::UpdateHealthBar(const float CurrentHealth, const float HealthChange)
{
	TargetHealthValue = CurrentHealth;
	GetWorld()->GetTimerManager().SetTimer(HealthBarAnimateTimer, this, &UPlayerHUD::LerpHealthBarValueToTarget, 0.01f, true);
}

void UPlayerHUD::UpdateManaBar(const float CurrentMana, const float ManaChange)
{
	TargetManaValue = CurrentMana;
	GetWorld()->GetTimerManager().SetTimer(ManaBarAnimateTimer, this, &UPlayerHUD::LerpManaBarValueToTarget, 0.01f, true);
}

void UPlayerHUD::BindRuneToSlot(int SlotIdx, URune* RuneCast)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;
	
	if (URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx]) {
		RuneCastSlot->SetRuneData(RuneCast);
	}
}

void UPlayerHUD::LerpHealthBarValueToTarget()
{
	const float TargetValuePercent = Player->Stats->GetHealth() / Player->Stats->GetMaxHealth();
	const float CurrentBarPercent = HealthBar->GetPercent();
	HealthBar->SetPercent( FMath::Lerp(CurrentBarPercent, TargetValuePercent, 0.5) );

	if (ManaBar->GetPercent() == TargetValuePercent)
	{
		HealthBar->SetPercent(TargetValuePercent);
		GetWorld()->GetTimerManager().ClearTimer(HealthBarAnimateTimer);
	}
}

void UPlayerHUD::LerpManaBarValueToTarget()
{
	const float TargetValuePercent = Player->Stats->GetMana() / Player->Stats->GetMaxMana();
	const float CurrentBarPercent = ManaBar->GetPercent();
	ManaBar->SetPercent( FMath::Lerp(CurrentBarPercent, TargetValuePercent, 0.5) );

	if (ManaBar->GetPercent() == TargetValuePercent)
	{
		ManaBar->SetPercent(TargetValuePercent);
		GetWorld()->GetTimerManager().ClearTimer(ManaBarAnimateTimer);
	}
}