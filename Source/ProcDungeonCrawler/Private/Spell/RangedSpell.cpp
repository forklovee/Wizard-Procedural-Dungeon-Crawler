// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/RangedSpell.h"

#include "Characters/Wizard/WizardCharacter.h"
#include "Components/SphereComponent.h"

ARangedSpell::ARangedSpell()
{
	SpellHitBoxComponent = CreateDefaultSubobject<USphereComponent>(FName("SpellHitBox"));
	SpellHitBoxComponent->SetupAttachment(RootComponent, NAME_None);

	StaticMeshHitBoxComponent = CreateDefaultSubobject<USphereComponent>(FName("StaticMeshHitBox"));
	StaticMeshHitBoxComponent->SetupAttachment(RootComponent, NAME_None);
	
	bRequireTarget = false;
}

void ARangedSpell::BeginPlay()
{
	Super::BeginPlay();
	
	SpellHitBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARangedSpell::OnSpellHitboxOverlapBegin);
	SpellHitBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ARangedSpell::OnSpellHitboxOverlapEnd);

	StaticMeshHitBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARangedSpell::OnStaticMeshHitboxOverlapBegin);
}

void ARangedSpell::CastSpell(AWizardCharacter* WizardCharacter)
{
	Super::CastSpell(WizardCharacter);
}

void ARangedSpell::OnStaticMeshHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SpellCaster.IsValid() || OtherActor->ActorHasTag(FName("SpellHandle"))) return;
	
	Destroy();
}

void ARangedSpell::OnSpellHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SpellCaster.IsValid()) return;
	UE_LOG(LogTemp, Warning, TEXT("AAASpell hitbox overlapped with %s!"), *OtherActor->GetName())

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
