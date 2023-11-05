// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuneCastsHistory.generated.h"

class URuneIcon;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API URuneCastsHistory : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void AddRuneToCastHistory(const URuneCast* RuneCast) const;
	UFUNCTION()
	void ClearCastHistory();

	UPROPERTY(meta=(BindWidget))
	UPanelWidget* RuneCastsHistoryPanel;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<URuneIcon> RuneIconClass;
};
