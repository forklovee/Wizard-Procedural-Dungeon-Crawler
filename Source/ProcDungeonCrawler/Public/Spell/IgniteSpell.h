// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell/SpellCast.h"
#include "IgniteSpell.generated.h"

/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API AIgniteSpell : public ASpellCast
{
	GENERATED_BODY()

public:
	AIgniteSpell();
	
	virtual void CastSpell(AWizardCharacter* WizardCharacter) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hitbox")
	class USphereComponent* SpellHitBoxComponent;
};
