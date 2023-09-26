// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell/RangedSpell.h"
#include "ProjectileSpell.generated.h"

class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API AProjectileSpell : public ARangedSpell
{
	GENERATED_BODY()

public:
	AProjectileSpell();

	virtual void CastSpell(AWizardCharacter* WizardCharacter) override;

protected:
	UPROPERTY(EditAnywhere, Category="Projectile", meta=(AllowPrivateAccess="true"))
	UProjectileMovementComponent* ProjectileMovementComponent;
};
