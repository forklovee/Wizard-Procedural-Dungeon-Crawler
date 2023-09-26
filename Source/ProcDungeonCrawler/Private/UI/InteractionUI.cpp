// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InteractionUI.h"

#include "Components/TextBlock.h"

void UInteractionUI::UpdateInteractionPrompt(FName ItemName, FName InteractionName)
{
	if (ItemName == NAME_None || InteractionName == NAME_None)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Visible);
		InteractionPromptText->SetText(FText::FromString(FString::Printf(TEXT("[E] %s %s"), *InteractionName.ToString(), *ItemName.ToString())));
	}
}
