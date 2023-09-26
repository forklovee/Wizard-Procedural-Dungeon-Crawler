// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/BasicProp.h"

// Sets default values
ABasicProp::ABasicProp()
{
	PrimaryActorTick.bCanEverTick = false;

	PropStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MainStaticMesh"));
	RootComponent = PropStaticMesh;
}

// SETUP SPELL INTERACTION
void ABasicProp::ChangeCollisionProfileToSpellInteractive_Implementation()
{
	ISpellHandleInterface::ChangeCollisionProfileToSpellInteractive_Implementation();

	PropStaticMesh->SetCollisionProfileName(FName("SpellInteractive"));
}

void ABasicProp::AddSpellHandleTag_Implementation()
{
	ISpellHandleInterface::AddSpellHandleTag_Implementation();

	Tags.Add(FName("SpellHandle"));
}
// SETUP SPELL INTERACTION

void ABasicProp::HandleSpellCast_Implementation(AWizardCharacter* WizardCharacter, ASpellCast* SpellCast)
{
	ISpellHandleInterface::HandleSpellCast_Implementation(WizardCharacter, SpellCast);
}



void ABasicProp::BeginPlay()
{
	Super::BeginPlay();

	// Setup basic spell interaction for prop
	ChangeCollisionProfileToSpellInteractive_Implementation();
	AddSpellHandleTag_Implementation();
}

// Called every frame
void ABasicProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

