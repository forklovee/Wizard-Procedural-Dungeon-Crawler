// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\UI\Wizard\PlayerHUD.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/Human/Player/PlayerPawn.h"
#include "Components/CanvasPanel.h"
#include "UI/Wizard/RuneCastSlot.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/Character/InventoryComponent.h"
#include "UI/Inventory/ItemTile.h"
#include "UI/Wizard/RuneCastsHistory.h"

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

	InventorySize = Player->Inventory->GetInventorySize();
	const int TargetSlotAmount = static_cast<int>(InventorySize.X * InventorySize.Y);
	UE_LOG(LogTemp, Display, TEXT("%f %f"), InventorySize.X, InventorySize.Y)
	UE_LOG(LogTemp, Display, TEXT("%d == %d"), TargetSlotAmount, ItemTiles.Num())
	if (TargetSlotAmount == ItemTiles.Num())
		return;
	
	for (int Y = 0; Y < static_cast<int>(InventorySize.Y); Y++)
	{
		for (int X = 0; X < static_cast<int>(InventorySize.X); X++)
		{
			UItemTile* ItemTile = CreateWidget<UItemTile>(GetWorld(), ItemTileClass);
			ItemTiles.Add(ItemTile);
			InventoryGrid->AddChildToUniformGrid(ItemTile, X, Y);
		}
	}
}

void UPlayerHUD::ToggleInventoryVisibility()
{
	SetInventoryVisible(!bIsVisible);
}

void UPlayerHUD::SetInventoryVisible(bool bNewIsVisible)
{
	bIsVisible = bNewIsVisible;
	
	InventoryCanvas->SetVisibility(!bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	// HudCanvas->SetVisibility(bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	
	if (OnInventoryVisibilityChanged.IsBound())
	{
		OnInventoryVisibilityChanged.Broadcast();
	}

	if (!bIsVisible)
	{
		return;
	}

	// for (FInventorySlot& InventorySlot : Player->Inventory->GetItems())
	// {
	// 	if (InventorySlot.ItemClass == nullptr) continue;
	// 	
	// 	UItemTile* ItemTile = CreateWidget<UItemTile>(GetWorld(), ItemTileClass);
	// 	ItemTile->UpdateData(InventorySlot);
	// 	ItemTile->OnUseItemRequest.AddDynamic(Player, &APlayerPawn::UseItem);
	// 	ItemTile->OnUseItemRequest.AddDynamic(Player->Inventory, &UInventoryComponent::RemoveItem);
	// 	// ItemTile->OnInspectItemRequest.AddDynamic(this, &UPlayerHUD::OnInspectItemRequest);
	// 	// ItemTile->OnDropItemRequest.AddDynamic(this, &UPlayerHUD::OnDropItemRequest);
	// 	InventoryGrid->AddChildToUniformGrid(ItemTile, InventorySlot.TilePos.X, InventorySlot.TilePos.Y);
	// }
}

void UPlayerHUD::OnItemAdded(int SlotIndex, FInventorySlot InventorySlot)
{
	ItemTiles[SlotIndex]->UpdateData(InventorySlot);
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

void UPlayerHUD::OnPlayerHeal(float Health, float HealHealh)
{
	if (OnPlayerHeal_UpdateHealth.IsBound())
	{
		OnPlayerHeal_UpdateHealth.Broadcast(Health, HealHealh);
	}
}

void UPlayerHUD::OnPlayerHurt(AActor* DamageCauser, float Damage, const UDamageType* DamageType)
{
	if (OnPlayerHurt_UpdateHealth.IsBound())
	{
		OnPlayerHurt_UpdateHealth.Broadcast(DamageCauser, Damage, DamageType);
	}
}

void UPlayerHUD::OnPlayerManaUsage(float Mana, float ManaUsage)
{
	if (OnPlayerManaUsage_ManaUpdate.IsBound())
	{
		OnPlayerManaUsage_ManaUpdate.Broadcast(Mana, ManaUsage);
	}
}



void UPlayerHUD::BindRuneToSlot(int SlotIdx, URune* RuneCast)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;
	
	if (URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx]) {
		RuneCastSlot->SetRuneData(RuneCast);
	}
}
