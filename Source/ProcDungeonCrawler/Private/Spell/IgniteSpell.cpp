// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/IgniteSpell.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"

AIgniteSpell::AIgniteSpell(): Super()
{
	SpellHitBoxComponent = CreateDefaultSubobject<USphereComponent>(FName("SpellHitBox"));
	SpellHitBoxComponent->SetupAttachment(RootComponent);
}

void AIgniteSpell::CastSpell(AWizardCharacter* WizardCharacter)
{
	Super::CastSpell(WizardCharacter);

}
