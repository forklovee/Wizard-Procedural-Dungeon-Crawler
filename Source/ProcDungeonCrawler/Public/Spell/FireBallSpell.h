// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileSpell.h"
#include "FireBallSpell.generated.h"

/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API AFireBallSpell : public AProjectileSpell
{
	GENERATED_BODY()

public:
	virtual void CastSpell(AWizardCharacter* WizardCharacter) override;
	
	virtual void OnSpellHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									const FHitResult& SweepResult) override;
	virtual void OnSpellHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
};
