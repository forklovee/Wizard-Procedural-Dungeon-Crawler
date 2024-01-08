// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Spell.generated.h"

class AHuman;
class URune;

USTRUCT(BlueprintType)
struct FSpellCastRuneChain: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASpell> SpellCast;

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<URune>> RuneChain;
};

UCLASS(BlueprintType, Blueprintable)
class PROCDUNGEONCRAWLER_API ASpell: public AActor
{
GENERATED_BODY()
	
public:
	ASpell();

	// Spell casting
	virtual void CastSpell(AHuman* WizardCharacter);
	virtual void ApplyEffectsOnTarget(AActor* TargetActor);

	// Actor targetting
	virtual bool CanActorBeTargeted(AActor* Actor) const;
	UFUNCTION(BlueprintCallable)
	virtual TSet<AActor*>& GetTargetedActors();
	virtual void TargetActor(AActor* Actor);
	virtual void UnTargetActor(AActor* Actor);
	
	AActor* GetFirstValidHitResultActor(TArray<FHitResult>& TargetedHitResults) const;
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spell")
	bool bRequireTarget = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spell")
	float ManaCost = 10.0f;
	
protected:
	UPROPERTY(EditAnywhere, Category="Particles", meta=(AllowPrivateAccess="true"))
	class USpellInteractionShapeComponent* SpellInteractionShape;
	
	UPROPERTY(EditAnywhere, Category="Particles", meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* SpellCastParticleSystemComponent;

	UPROPERTY()
	TWeakObjectPtr<AHuman> SpellCaster;
	
private:
	UPROPERTY()
	TSet<AActor*> TargetedActors;
};
