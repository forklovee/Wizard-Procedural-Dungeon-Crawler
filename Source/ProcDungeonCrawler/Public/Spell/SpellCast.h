// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpellCast.generated.h"

class AWizardCharacter;
class URuneCast;

USTRUCT(BlueprintType)
struct FSpellCastRuneChain: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USpellCast> SpellCastClass;

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<URuneCast>> RuneChain;
};

UCLASS(BlueprintType, Blueprintable)
class PROCDUNGEONCRAWLER_API USpellCast: public UObject
{
GENERATED_BODY()
	
public:
	USpellCast();
	
	virtual void CastSpell(AWizardCharacter* WizardCharacter);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle System")
	TSoftObjectPtr<UParticleSystem> SpellCastParticleSystem;
};
