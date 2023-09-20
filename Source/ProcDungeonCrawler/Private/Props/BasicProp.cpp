// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/BasicProp.h"

// Sets default values
ABasicProp::ABasicProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABasicProp::HandleSpellCast_Implementation(AWizardCharacter* WizardCharacter, ASpellCast* SpellCast)
{
	ISpellHandleInterface::HandleSpellCast_Implementation(WizardCharacter, SpellCast);
}

// Called when the game starts or when spawned
void ABasicProp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

