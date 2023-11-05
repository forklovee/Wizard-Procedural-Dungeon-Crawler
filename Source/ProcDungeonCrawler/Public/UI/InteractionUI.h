// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionUI.generated.h"

class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewInteractionItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionItemLost);

UCLASS()
class PROCDUNGEONCRAWLER_API UInteractionUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionItem OnNewInteractionItem;
	UPROPERTY(BlueprintAssignable)
	FOnInteractionItemLost OnInteractionItemLost;
	
	UFUNCTION(BlueprintCallable)
	void UpdateInteractionPrompt(FName ItemName, FName InteractionName);

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* InteractionPromptText;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* InteractionKeyImage;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* CursorImage;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* InteractionImage;

private:
	FName LastItemName = FName("ItemName");
};
