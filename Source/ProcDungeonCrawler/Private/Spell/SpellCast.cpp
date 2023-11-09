// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Spell\SpellCast.h"

#include "Characters/Wizard/WizardCharacter.h"
#include "NiagaraComponent.h"
#include "Interface/SpellHandleInterface.h"

ASpellCast::ASpellCast()
{
	SpellCastParticleSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("SpellParticles"));
	SpellCastParticleSystemComponent->bAutoActivate = false;

	RootComponent = SpellCastParticleSystemComponent;
	
	UE_LOG(LogTemp, Display, TEXT("%s SpellCast created!"), *GetName())
}

ASpellCast::~ASpellCast()
{
	UE_LOG(LogTemp, Display, TEXT("%s SpellCast destroyed!"), *GetName())
}

void ASpellCast::CastSpell(AWizardCharacter* WizardCharacter)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	SpellCaster = WizardCharacter;
	
	if (SpellCastParticleSystemComponent->GetAsset() != nullptr)
	{
		SpellCastParticleSystemComponent->Activate();
	}
}

void ASpellCast::ApplyEffectsOnTarget(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("ASpellCast::ApplyEffectsOnTarget - %s"), *TargetActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Is spellcaster valid? %i"), SpellCaster.IsValid());
	
	if (SpellCaster.IsValid() && IsImplementingSpellHandle(TargetActor))
	{
		ISpellHandleInterface::Execute_HandleSpellCast(TargetActor, SpellCaster.Get(), this);
	}
}

bool ASpellCast::CanActorBeTargeted(AActor* Actor) const
{
	return true;
}

TSet<AActor*>& ASpellCast::GetTargetedActors()
{
	return TargetedActors;
}

void ASpellCast::TargetActor(AActor* Actor)
{
	TargetedActors.Add(Actor);
}

void ASpellCast::UnTargetActor(AActor* Actor)
{
	TargetedActors.Remove(Actor);
}

bool ASpellCast::IsImplementingSpellHandle(AActor* Actor) const
{
	return Cast<ISpellHandleInterface>(Actor) != nullptr;
}

AActor* ASpellCast::GetFirstValidHitResultActor(TArray<FHitResult>& TargetedHitResults) const
{
	for (FHitResult HitResult : TargetedHitResults)
	{
		if (IsImplementingSpellHandle(HitResult.GetActor()))
		{
			return HitResult.GetActor();
		}
	}
	return nullptr;
}
