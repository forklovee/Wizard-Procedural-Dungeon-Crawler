// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Rune.h"
#include "Spell/RuneCast.h"

ARune::ARune()
{
	PrimaryActorTick.bCanEverTick = false;
}

TSoftObjectPtr<URuneCast>& ARune::GetRuneCast()
{
	return RuneCast;
}

FText ARune::GetPropNameText_Implementation()
{
	if (RuneCast.IsValid())
	{
		return FText::FromName(RuneCast->RuneName);
	}
	return FText::FromString("NONE");
}

TSubclassOf<AActor> ARune::Pickup_Implementation(APawn* Pawn)
{
	IPropPickupInterface::Pickup_Implementation(Pawn);

	const TSubclassOf<AActor> ItemClass = GetClass();
	
	Destroy();
	
	return ItemClass;
}

UDataAsset* ARune::GetAdditionalDataAsset_Implementation()
{
	IPropPickupInterface::GetAdditionalDataAsset_Implementation();

	if (RuneCast.IsNull())
	{
		return nullptr;
	}
	
	return RuneCast.LoadSynchronous();
}
