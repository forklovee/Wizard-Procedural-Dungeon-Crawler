// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Wizard/WizardHUD.h"

#include "UI/Wizard/RuneCastSlotsContainer.h"

void UWizardHUD::AddRuneToUI(URuneCast* RuneCast)
{
	if (RuneCastSlotsContainer)
	{
		RuneCastSlotsContainer->AddNewRune(RuneCast);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Rune cast slots container is null!"));
	}
}

void UWizardHUD::OpenBag(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Open Bag!"));
}

void UWizardHUD::OpenMap(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Open Map!"));
}

void UWizardHUD::OpenSpellbook(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("Open Spellbook!"));
}

void UWizardHUD::SetupRuneCastSlotsContainer()
{
	if (RuneCastSlotsContainer)
	{
		RuneCastSlotsContainer->RegisterAllRunCastSlots();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("chuj"));
	}
}
