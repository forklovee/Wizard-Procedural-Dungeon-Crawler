// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InteractionUI.h"

#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Character/PlayerInteractionRaycast.h"

void UInteractionUI::ClearInteractionPrompt()
{
	UpdateInteractionPrompt(FText::FromString(""), FName(""), false);
}

void UInteractionUI::UpdateInteractionPrompt(FText ActorNameText, FName InteractionName, bool bCanBeGrabbed)
{
	InteractionActorNameText->SetText(ActorNameText);
	InteractionPromptText->SetText(FText::FromString(InteractionName.ToString()));

	InteractionPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
	
	if (InteractionName == FName("Interact") || InteractionName == FName("Pickup"))
	{
		InteractionPromptPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractionPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	GrabPromptPanel->SetVisibility(bCanBeGrabbed ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
