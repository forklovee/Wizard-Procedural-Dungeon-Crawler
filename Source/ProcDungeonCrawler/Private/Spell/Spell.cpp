// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/Spell.h"
#include "Characters/Human/Human.h"

#include "NiagaraComponent.h"
#include "Components/Props/SpellInteractionShapeComponent.h"

ASpell::ASpell()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	SpellCastParticleSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("SpellParticles"));
	SpellCastParticleSystemComponent->bAutoActivate = false;
	SpellCastParticleSystemComponent->SetupAttachment(RootComponent);

	SpellInteractionShape = CreateDefaultSubobject<USpellInteractionShapeComponent>(FName("SpellInteraction"));
	SpellInteractionShape->SetupAttachment(RootComponent);
	
	UE_LOG(LogTemp, Display, TEXT("%s SpellCast created!"), *GetName())
}

void ASpell::CastSpell(AHuman* WizardCharacter)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	SpellCaster = WizardCharacter;
	
	if (SpellCastParticleSystemComponent->GetAsset() != nullptr)
	{
		SpellCastParticleSystemComponent->Activate();
	}
}

void ASpell::ApplyEffectsOnTarget(AActor* TargetActor)
{
	UE_LOG(LogTemp, Warning, TEXT("ASpell::ApplyEffectsOnTarget - %s"), *TargetActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Is spellcaster valid? %i"), SpellCaster.IsValid());
	
	// if (SpellCaster.IsValid() && IsImplementingSpellHandle(TargetActor))
	// {
	// 	ISpellHandleInterface::Execute_HandleSpellCast(TargetActor, SpellCaster.Get(), this);
	// }
}

bool ASpell::CanActorBeTargeted(AActor* Actor) const
{
	return true;
}

TSet<AActor*>& ASpell::GetTargetedActors()
{
	return TargetedActors;
}

void ASpell::TargetActor(AActor* Actor)
{
	TargetedActors.Add(Actor);
}

void ASpell::UnTargetActor(AActor* Actor)
{
	TargetedActors.Remove(Actor);
}

AActor* ASpell::GetFirstValidHitResultActor(TArray<FHitResult>& TargetedHitResults) const
{
	// for (FHitResult HitResult : TargetedHitResults)
	// {
	// 	if (IsImplementingSpellHandle(HitResult.GetActor()))
	// 	{
	// 		return HitResult.GetActor();
	// 	}
	// }
	return nullptr;
}
