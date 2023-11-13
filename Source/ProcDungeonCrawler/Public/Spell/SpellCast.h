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

	// Spell casting
	virtual void CastSpell(AWizardCharacter* WizardCharacter);
	virtual void ApplyEffectsOnTarget(AActor* TargetActor);

	// Actor targetting
	virtual bool CanActorBeTargeted(AActor* Actor) const;
	UFUNCTION(BlueprintCallable)
	virtual TSet<AActor*>& GetTargetedActors();
	virtual void TargetActor(AActor* Actor);
	virtual void UnTargetActor(AActor* Actor);
	
	bool IsImplementingSpellHandle(AActor* Actor) const;
	AActor* GetFirstValidHitResultActor(TArray<FHitResult>& TargetedHitResults) const;
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spell")
	bool bRequireTarget = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spell")
	float ManaCost = 10.0f;
	
protected:
	UPROPERTY(EditAnywhere, Category="Particles", meta=(AllowPrivateAccess="true"))
	class UNiagaraComponent* SpellCastParticleSystemComponent;

	UPROPERTY()
	TWeakObjectPtr<AWizardCharacter> SpellCaster;
	
private:
	UPROPERTY()
	TSet<AActor*> TargetedActors;
};
