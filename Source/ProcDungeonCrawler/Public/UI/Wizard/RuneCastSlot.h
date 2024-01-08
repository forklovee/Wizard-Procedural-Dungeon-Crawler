// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "..\..\Spell\Rune.h"
#include "RuneCastSlot.generated.h"

class UTextBlock;
class UImage;
class URune;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTriggerSlotAnimation, int, RuneIdx);

UCLASS()
class PROCDUNGEONCRAWLER_API URuneCastSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTriggerSlotAnimation OnTriggerSlotAnimation;

	UFUNCTION(BlueprintCallable)
	void SetRuneData(URune* RuneCast);
	void ClearRuneData();
	void SetSlotIndex(int Index);

	UFUNCTION(BlueprintCallable)
	URune* UseRune();
	
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
	UPROPERTY()
	URune* RuneCast;
	
	TSoftObjectPtr<UTexture2D> RuneIcon;
	FText RuneName;
	int RuneIndex;
};
