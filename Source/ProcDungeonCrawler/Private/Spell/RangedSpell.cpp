// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/RangedSpell.h"

#include "Components/SphereComponent.h"

ARangedSpell::ARangedSpell()
{
	SpellHitBoxComponent = CreateDefaultSubobject<USphereComponent>(FName("SpellHitBox"));
	SpellHitBoxComponent->SetupAttachment(RootComponent, NAME_None);

	bRequireTarget = false;
}

void ARangedSpell::BeginPlay()
{
	Super::BeginPlay();
	
	SpellHitBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARangedSpell::OnSpellHitboxOverlapBegin);
	SpellHitBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ARangedSpell::OnSpellHitboxOverlapEnd);
}

void ARangedSpell::CastSpell(AWizardCharacter* WizardCharacter)
{
	Super::CastSpell(WizardCharacter);
}

void ARangedSpell::OnSpellHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SpellCaster.IsValid()) return;
	
	if (OtherActor->ActorHasTag(FName("SpellHandle")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Spell hitbox overlapped with %s!"), *OtherActor->GetName())
		ApplyEffectsOnTarget(OtherActor);
	}
}

void ARangedSpell::OnSpellHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!SpellCaster.IsValid()) return;
	
	if (OtherActor->ActorHasTag(FName("SpellHandle")))
	{
		
	}
}
