// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/WizardPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "UI/Wizard/WizardHUD.h"


void AWizardPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

UWizardHUD* AWizardPlayerController::SetupWizardHud()
{
	if (WizardHUD)
	{
		WizardHUD->RemoveFromParent();
	}
	
	if (WizardHUDClass)
	{
		WizardHUD = Cast<UWizardHUD>(CreateWidget<UUserWidget>(this, WizardHUDClass));
		if (WizardHUD)
		{
			WizardHUD->AddToViewport();
		}
	}

	return WizardHUD;
}
