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
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UWizardHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void UseRuneOfIdx(int Idx);

	bool IsSlotEmpty(int SlotIdx) const;
	UFUNCTION()
	void BindRuneToSlot(int SlotIdx, URuneCast* RuneCast);
	UFUNCTION()
	void UnbindRuneToSlot(int SlotIdx);

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
