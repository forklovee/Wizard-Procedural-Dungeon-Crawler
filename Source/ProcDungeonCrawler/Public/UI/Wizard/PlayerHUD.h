// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UProgressBar;
class UCanvasPanel;
class UItemTile;
class UUniformGridPanel;
class APlayerPawn;
class AItem;
class UPawnStats;
class URuneCastsHistory;
class UInteractionUI;
class URune;
class UHorizontalBox;
class URuneCastSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeal_HealthUpdate, float, Health, float, HealHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHurt_HealthUpdate, AActor*, DamageCauser, float, Damage, const UDamageType*, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerManaUsage_ManaUpdate, float, Mana, float, ManaUsage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractionTargetChanged, FText, ActorName, FName, InteractionType, bool, bCanBeGrabbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryVisibilityChanged, bool, bNewIsInventoryVisible);

UCLASS()
class PROCDUNGEONCRAWLER_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractionTargetChanged OnInteractionTargetChanged;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryVisibilityChanged OnInventoryVisibilityChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnHeal_HealthUpdate OnPlayerHeal_UpdateHealth;
	UPROPERTY(BlueprintAssignable)
	FOnHurt_HealthUpdate OnPlayerHurt_UpdateHealth;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerManaUsage_ManaUpdate OnPlayerManaUsage_ManaUpdate;

	void SetupInventory(APlayerPawn* PlayerPawn);
	
	UFUNCTION(BlueprintCallable)
	void ToggleInventoryVisibility();
	
	UFUNCTION(BlueprintCallable)
	void SetInventoryVisible(bool bNewIsVisible);

	UFUNCTION()
	void UpdateInventorySlot(int SlotIndex, FInventorySlot InventorySlot);
	
	UFUNCTION(BlueprintCallable)
	void OnNewInteractionTarget(FText ActorName, FName InteractionType, bool bCanBeGrabbed);
	
	UFUNCTION()
	void OnRuneCasted(int RuneIdx, URune* RuneCast, TArray<URune*>& CastedRunes);

	bool IsSlotEmpty(int SlotIdx) const;
	UFUNCTION()
	void BindRuneToSlot(int SlotIdx, URune* RuneCast);
	UFUNCTION()
	void UnbindRuneToSlot(int SlotIdx);

	UFUNCTION()
	void UpdateHealthBar(const float CurrentHealth, const float HealthChange);

	UFUNCTION()
	void UpdateManaBar(const float CurrentMana, const float ManaChange);

protected:
	virtual void NativeOnInitialized() override;

	void UpdateInventoryData();
	UItemTile* GetItemTileAtPosition(FVector2D TilePos) const;

private:
	void LerpHealthBarValueToTarget();
	void LerpManaBarValueToTarget();
	
public:
	UPROPERTY()
	APlayerPawn* Player;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UItemTile> ItemTileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UCanvasPanel* HudCanvas;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UCanvasPanel* InventoryCanvas;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Stats", meta=(BindWidget))
	UProgressBar* HealthBar;
	FTimerHandle HealthBarAnimateTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Stats", meta=(BindWidget))
	UProgressBar* ManaBar;
	FTimerHandle ManaBarAnimateTimer;
	
// public:
// 	UPROPERTY(EditAnywhere, meta=(BindWidget))
// 	UHorizontalBox* RuneSlots;
//
// 	UPROPERTY(EditAnywhere, meta = (BindWidget))
// 	URuneCastsHistory* CastedRuneHistory;
//
// 	UPROPERTY(EditAnywhere, meta = (BindWidget))
// 	UInteractionUI* InteractionUI;

protected:
	TArray<UItemTile*> ItemTiles;
	TArray<URuneCastSlot*> RuneCastSlots;

	FVector2D InventorySize = FVector2D(5, 5);
	bool bInventoryIsVisible = false;

private:
	float TargetHealthValue = 0.f;
	float TargetManaValue = 0.f;
};
