// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WizardHUD.generated.h"

class URuneCast;
class URuneCastSlotsContainer;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UWizardHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddRuneToUI(URuneCast* RuneCast);

	void OpenBag(const FInputActionValue& Value);
	void OpenMap(const FInputActionValue& Value);
	void OpenSpellbook(const FInputActionValue& Value);
	
	void SetupRuneCastSlotsContainer();
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	URuneCastSlotsContainer* RuneCastSlotsContainer;

};
