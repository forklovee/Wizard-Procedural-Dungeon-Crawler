// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuneIcon.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API URuneIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRuneIcon(UTexture2D* Icon);
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* RuneIcon;
};
