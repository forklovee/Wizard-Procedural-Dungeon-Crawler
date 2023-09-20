// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Spell\SpellCast.h"

#include "WizardCharacter.h"
#include "NiagaraComponent.h"

ASpellCast::ASpellCast()
{
	SpellCastParticleSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("SpellParticles"));
	SpellCastParticleSystemComponent->SetupAttachment(RootComponent);
	SpellCastParticleSystemComponent->bAutoActivate = false;
}

ASpellCast::~ASpellCast()
{
	UE_LOG(LogTemp, Display, TEXT("%s SpellCast destroyed!"), *GetName())
}

void ASpellCast::CastSpell(AWizardCharacter* WizardCharacter)
{
	if (SpellCastParticleSystemComponent->GetAsset() != nullptr)
	{
		SpellCastParticleSystemComponent->Activate();
	}
}

void ASpellCast::CastSpell(AWizardCharacter* WizardCharacter, AActor* TargetActor)
{
	if (TargetActor)
	{
		UE_LOG(LogTemp, Display, TEXT("Casting spell on a target of %s!"), *TargetActor->GetName());
	}
}
