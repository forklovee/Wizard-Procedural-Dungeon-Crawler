// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionUI.generated.h"

class UProgressBar;
enum class EInteractionType;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractionItem, FText, ActorNameText, bool, bCanBeGrabbed);
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
	void ClearInteractionPrompt();

	UFUNCTION()
	void UpdateInteractionPrompt(FText ActorNameText, EInteractionType OnClickInteractionType, bool bCanBeGrabbed);

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* InteractionActorNameText;

	// Interaction
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UPanelWidget* InteractionPromptPanel;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* InteractionPromptText;

	// Grab
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UPanelWidget* GrabPromptPanel;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UProgressBar* GrabItemProgressBar;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* CursorImage;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* InteractionImage;
	
};
