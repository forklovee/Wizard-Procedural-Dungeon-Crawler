// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuneCastSlotsContainer.generated.h"

class UHorizontalBox;
class URune;
class URuneCastSlot;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API URuneCastSlotsContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;
	
	UFUNCTION(BlueprintCallable)
	void AddNewRune(URune* RuneCast);
	
	UFUNCTION(BlueprintCallable)
	void RegisterAllRunCastSlots();
	
	UFUNCTION(BlueprintCallable)
	void ChangeVisibleSlot(int SlotIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VisibleSlot = 0;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UHorizontalBox* RuneSlotContainer1;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UHorizontalBox* RuneSlotContainer2;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UHorizontalBox* RuneSlotContainer3;

private:
	TArray<URuneCastSlot*> RuneCastSlots;
	TArray<int> RuneCastAvailableSlotIds;
};
