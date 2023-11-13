// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UStatusEffect : public UObject
{
	GENERATED_BODY()

public:
	void ApplyEffect(APawn* Pawn);
};
