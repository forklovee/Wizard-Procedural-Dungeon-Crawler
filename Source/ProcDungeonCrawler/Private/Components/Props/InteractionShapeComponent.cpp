// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Props/InteractionShapeComponent.h"

#include "Characters/Human/Human.h"

UInteractionShapeComponent::UInteractionShapeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInteractionShapeComponent::Interact(AHuman* Pawn)
{
	if (RequiredItems.Num() > 0)
	{
		if (const AHuman* Human = Cast<AHuman>(Pawn))
		{
			for (const TSubclassOf<AItem>& RequiredItemClass: RequiredItems)
			{
				int OutAmount = 0;
				if (!Human->Inventory->HasItemOfClass(RequiredItemClass, OutAmount))
				{
					if (OnInteractionFailed.IsBound())
					{
						OnInteractionFailed.Broadcast(Pawn);
					}
					return false;
				}
			}
			
			if (bConsumeRequiredItems)
			{
				for (const TSubclassOf<AItem>& RequiredItemClass: RequiredItems)
				{
					Human->Inventory->RemoveItemByClass(RequiredItemClass);
				}
			}
		}
	}
	
	if (OnInteractionSuccessful.IsBound())
	{
		OnInteractionSuccessful.Broadcast(Pawn);
	}
	
	return true;
}


void UInteractionShapeComponent::BeginPlay()
{
	Super::BeginPlay();
}
