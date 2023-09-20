// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/WoodenBranches.h"

#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"

#include "Spell/IgniteSpell.h"
#include "Spell/SpellCast.h"

AWoodenBranches::AWoodenBranches()
{
	WoodenBranchesStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BranchesMesh"));
	RootComponent = WoodenBranchesStaticMesh;
	
	FireDamageCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("FireDamageCollision"));
	FireDamageCollision->SetCollisionProfileName(FName("OverlapAll"));
	FireDamageCollision->SetupAttachment(RootComponent);

	FireWarmthRangeCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("FireWarmthCollision"));
	FireWarmthRangeCollision->SetCollisionProfileName(FName("OverlapAll"));
	FireWarmthRangeCollision->SetupAttachment(RootComponent);
	
	SpellCastParticleSystemComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("FireParticles"));
	SpellCastParticleSystemComponent->SetupAttachment(RootComponent);
}

void AWoodenBranches::BeginPlay()
{
	Super::BeginPlay();
}

void AWoodenBranches::HandleSpellCast_Implementation(AWizardCharacter* WizardCharacter, ASpellCast* SpellCast)
{
	Super::HandleSpellCast_Implementation(WizardCharacter, SpellCast);

	if (!bIsLit && SpellCast->GetClass() == AIgniteSpell::StaticClass())
	{
		UE_LOG(LogTemp, Display, TEXT("WoodenBranches::HandleSpellCast_Implementation - IgniteSpell"));
		bIsLit = true;

		FireDamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AWoodenBranches::OnFireAffectRangeBeginOverlap);
		FireDamageCollision->OnComponentEndOverlap.AddDynamic(this, &AWoodenBranches::OnFireAffectRangeEndOverlap);
	}
}

void AWoodenBranches::OnFireAffectRangeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AWoodenBranches::OnFireAffectRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}
