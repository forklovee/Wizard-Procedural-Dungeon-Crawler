// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "WizardPlayerController.generated.h"

class UWizardHUD;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API AWizardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UWizardHUD* SetupWizardHud();
	UWizardHUD* GetWizardHud() const { return WizardHUD; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UWizardHUD> WizardHUDClass;

	UPROPERTY(EditAnywhere)
	UWizardHUD* WizardHUD;
};
