// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Spell/RuneCast.h"
#include "RuneCastSlot.generated.h"

class UTextBlock;
class UImage;
class AWizardCharacter;
class URuneCast;

UCLASS()
class PROCDUNGEONCRAWLER_API URuneCastSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetRuneData(int RuneIndex, URuneCast* RuneCast);

	void SetRuneCastSlotIndex(int Index);
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void TriggerUseAnimation(AWizardCharacter* WizardCharacter, URuneCast* RuneCast);

	UFUNCTION(BlueprintCallable)
	bool IsEmpty();

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* RuneIconImage;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* RuneNameTextBlock;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* RuneIdTextBlock;

private:
	TSoftObjectPtr<URuneCast> RuneCast;
	
	TSoftObjectPtr<UTexture2D> RuneIcon;
	FName RuneName = "Empty";
	int RuneIndex = 0;
};
