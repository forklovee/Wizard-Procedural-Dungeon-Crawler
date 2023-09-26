// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionUI.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UInteractionUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateInteractionPrompt(FName ItemName, FName InteractionName);

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* InteractionPromptText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* CursorImage;
};
