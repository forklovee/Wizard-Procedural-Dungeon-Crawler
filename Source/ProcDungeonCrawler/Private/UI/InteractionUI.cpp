// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InteractionUI.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInteractionUI::UpdateInteractionPrompt(FName ItemName, FName InteractionName)
{
	if (LastItemName == ItemName) return;
	
	LastItemName = ItemName;
	
	if (ItemName == NAME_None || InteractionName == NAME_None)
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
		InteractionKeyImage->SetVisibility(ESlateVisibility::Collapsed);

		if (OnInteractionItemLost.IsBound()){
			OnInteractionItemLost.Broadcast();
		}
	}
	else
	{
		InteractionPromptText->SetVisibility(ESlateVisibility::Visible);
		InteractionPromptText->SetText(FText::FromName(ItemName));
		InteractionKeyImage->SetVisibility(ESlateVisibility::Visible);

		if (OnNewInteractionItem.IsBound()){
			OnNewInteractionItem.Broadcast();
		}
	}
}
