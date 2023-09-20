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
	TSubclassOf<ASpellCast> SpellCast;

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<URuneCast>> RuneChain;
};

UCLASS(BlueprintType, Blueprintable)
class PROCDUNGEONCRAWLER_API ASpellCast: public AActor
{
GENERATED_BODY()
	
public:
	ASpellCast();
	~ASpellCast();
	
	virtual void CastSpell(AWizardCharacter* WizardCharacter);
	virtual void CastSpell(AWizardCharacter* WizardCharacter, AActor* TargetActor);

public:
	UPROPERTY(EditAnywhere, Category="Spell")
	bool bRequireTarget = true;
	
protected:
	UPROPERTY(EditAnywhere, Category="Particles", meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* SpellCastParticleSystemComponent;
};
