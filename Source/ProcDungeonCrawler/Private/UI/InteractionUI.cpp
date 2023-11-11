// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InteractionUI.h"

#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Character/PlayerInteractionRaycast.h"

void UInteractionUI::ClearInteractionPrompt()
{
	UpdateInteractionPrompt(FText::FromString(""), EInteractionType::E_None, false);
}

void UInteractionUI::UpdateInteractionPrompt(FText ActorNameText, EInteractionType OnClickInteractionType, bool bCanBeGrabbed)
{
	InteractionActorNameText->SetText(ActorNameText);
	if (OnClickInteractionType == EInteractionType::E_None && !bCanBeGrabbed)
	{
		InteractionPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
		GrabPromptPanel->SetVisibility(ESlateVisibility::Collapsed);

		if (OnInteractionItemLost.IsBound()){
			OnInteractionItemLost.Broadcast();
		}
	}
	else
	{
		switch (OnClickInteractionType)
		{
		case EInteractionType::E_None:
			InteractionPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
			break;
		case EInteractionType::E_Interact:
			InteractionPromptPanel->SetVisibility(ESlateVisibility::Visible);
			InteractionPromptText->SetText(FText::FromString("Interact"));
			break;
		case EInteractionType::E_Pickup:
			InteractionPromptPanel->SetVisibility(ESlateVisibility::Visible);
			InteractionPromptText->SetText(FText::FromString("Pickup"));
			break;
		default:
			InteractionPromptPanel->SetVisibility(ESlateVisibility::Collapsed);
			break;
		}

		GrabPromptPanel->SetVisibility(bCanBeGrabbed ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		if (OnNewInteractionItem.IsBound()){
			OnNewInteractionItem.Broadcast(ActorNameText, bCanBeGrabbed);
		}
	}
}
