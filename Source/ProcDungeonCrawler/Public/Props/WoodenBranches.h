// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Props/BasicProp.h"
#include "WoodenBranches.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API AWoodenBranches : public ABasicProp
{
	GENERATED_BODY()

public:
	AWoodenBranches();

	virtual void BeginPlay() override;
	
	virtual void HandleSpellCast_Implementation(class AWizardCharacter* WizardCharacter, class ASpellCast* SpellCast) override;

protected:
	void OnFireAffectRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                   const FHitResult& SweepResult);

	void OnFireAffectRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(EditAnywhere, Category="Damage")
	float MinBurnDistance = 10.f;

	UPROPERTY(EditAnywhere, Category="Damage")
	float BurnDamage = 1.f;
	
protected:
	UPROPERTY(EditAnywhere, Category="Particles", meta=(AllowPrivateAccess="true"))
	UNiagaraComponent* SpellCastParticleSystemComponent;

	UPROPERTY(EditAnywhere, Category="Damage", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* FireDamageCollision;

	UPROPERTY(EditAnywhere, Category="Damage", meta=(AllowPrivateAccess="true"))
	UCapsuleComponent* FireWarmthRangeCollision;
	
	UPROPERTY(EditAnywhere, Category="Mesh", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* WoodenBranchesStaticMesh;

	bool bIsLit = false;
	
};
