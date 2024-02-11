// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\UI\Wizard\PlayerHUD.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/Human/Player/DefaultPlayerController.h"
#include "Characters/Human/Player/PlayerPawn.h"
#include "Components/CanvasPanel.h"
#include "UI/Wizard/RuneCastSlot.h"
#include "Components/ProgressBar.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/PawnStats.h"
#include "UI/Inventory/ItemTile.h"
#include "UI/Inventory/ItemTileContextMenu.h"
#include "UI/Inventory/Tabs/InventoryTabButton.h"

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetInventoryVisible(false);
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
	
	for (FInventorySlot& InventorySlot: PlayerPawn->Inventory->GetItemSlots())
	{
		UItemTile* ItemTile = CreateWidget<UItemTile>(GetWorld(), ItemTileClass);
		ItemTile->OnUseItemRequest.AddDynamic(PlayerPawn->Inventory, &UInventoryComponent::UseItem);
		ItemTile->OnDropItemRequest.AddDynamic(PlayerPawn->Inventory, &UInventoryComponent::DropItem);
		
		ItemTile->OnContextMenuRequested.AddDynamic(InventoryContextMenu, &UItemTileContextMenu::Open);
		
		ItemTiles.Add(ItemTile);
		
		InventoryGrid->AddChildToUniformGrid(ItemTile, InventorySlot.TilePos.X, InventorySlot.TilePos.Y);
		ItemTile->SetInventorySlot(InventorySlot);
	}
}

void UPlayerHUD::ToggleInventoryVisibility()
{
	SetInventoryVisible(!bInventoryIsVisible);
}

void UPlayerHUD::SetInventoryVisible(bool bNewIsVisible)
{
	bInventoryIsVisible = bNewIsVisible;
	UE_LOG(LogTemp, Display, TEXT("Inventory visible: %i"), bInventoryIsVisible)
	InventoryCanvas->SetVisibility(!bInventoryIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	if (Player)
		if (ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(Player->GetController()))
		{
			PlayerController->bShowMouseCursor = bInventoryIsVisible;
			if (bInventoryIsVisible)
			{
				PlayerController->SetInputMode(FInputModeGameAndUI());
			}
			else
			{
				PlayerController->SetInputMode(FInputModeGameOnly());
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to get PlayerController"))
		}

	
	if (OnInventoryVisibilityChanged.IsBound())
	{
		OnInventoryVisibilityChanged.Broadcast(bInventoryIsVisible);
	}
}

void UPlayerHUD::UpdateInventorySlot(FInventorySlot& InventorySlot)
{
	ItemTiles[InventorySlot.Index]->UpdateVisualData();
	// SetInventorySlot(InventorySlot);
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

void UPlayerHUD::OnInventoryTabButtonPressed()
{
	InventoryTabSwitcher->SetActiveWidgetIndex(0);
}

void UPlayerHUD::OnSpellBookTabButtonPressed()
{
	InventoryTabSwitcher->SetActiveWidgetIndex(1);
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