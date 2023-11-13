// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WizardHUD.generated.h"

class URuneCastsHistory;
class UInteractionUI;
class URuneCast;
class UHorizontalBox;
class URuneCastSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeal_HealthUpdate, float, Health, float, HealHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHurt_HealthUpdate, AActor*, DamageCauser, float, Damage, UDamageType*, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerManaUsage_ManaUpdate, float, Mana, float, ManaUsage);

UCLASS()
class PROCDUNGEONCRAWLER_API UWizardHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnHeal_HealthUpdate OnPlayerHeal_UpdateHealth;
	UPROPERTY(BlueprintAssignable)
	FOnHurt_HealthUpdate OnPlayerHurt_UpdateHealth;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerManaUsage_ManaUpdate OnPlayerManaUsage_ManaUpdate;
	
	UFUNCTION()
	void UseRuneOfIdx(int Idx);

	bool IsSlotEmpty(int SlotIdx) const;
	UFUNCTION()
	void BindRuneToSlot(int SlotIdx, URuneCast* RuneCast);
	UFUNCTION()
	void UnbindRuneToSlot(int SlotIdx);

	UFUNCTION()
	void OnPlayerHeal(float Health, float HealHealh);
	UFUNCTION()
	void OnPlayerHurt(AActor* DamageCauser, float Damage, UDamageType* DamageType);
	UFUNCTION()
	void OnPlayerManaUsage(float Mana, float ManaUsage);
	
protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UHorizontalBox* RuneSlots;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	URuneCastsHistory* CastedRuneHistory;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UInteractionUI* InteractionUI;

protected:
	TArray<URuneCastSlot*> RuneCastSlots;
};
