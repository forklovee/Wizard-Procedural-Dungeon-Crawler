// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIBagItem.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class UUIBagItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(TSubclassOf<AActor> ItemClass, int32 Amount);
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Frame;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* ItemImage;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ItemName;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ItemAmount;
};
