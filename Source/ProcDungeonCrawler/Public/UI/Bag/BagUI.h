// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BagUI.generated.h"

class UScrollBox;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UBagUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UScrollBox* MainGridContainer;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UUniformGridPanel* ItemGrid;
};
