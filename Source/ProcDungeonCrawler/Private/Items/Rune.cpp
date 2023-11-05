// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Rune.h"
#include "Spell/RuneCast.h"

TSoftObjectPtr<URuneCast>& ARune::GetRuneCast()
{
	return RuneCast;
}

UDataAsset* ARune::GetAdditionalDataAsset_Implementation()
{
	Super::GetAdditionalDataAsset_Implementation();

	if (RuneCast.IsNull())
	{
		return nullptr;
	}
	
	return RuneCast.LoadSynchronous();
}

FName ARune::GetItemName_Implementation()
{
	FName RuneName = "Rune";
	if (RuneCast.IsValid())
	{
		RuneName = RuneCast->RuneName;
	}
	return RuneName;
}

FName ARune::GetInteractionName_Implementation()
{
	return FName("Pick up");
}
