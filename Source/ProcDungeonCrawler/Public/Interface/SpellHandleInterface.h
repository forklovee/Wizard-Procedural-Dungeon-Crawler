// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpellHandleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USpellHandleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROCDUNGEONCRAWLER_API ISpellHandleInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spell")
	void HandleSpellCast(class AWizardCharacter* WizardCharacter, class ASpellCast* SpellCast);
};
