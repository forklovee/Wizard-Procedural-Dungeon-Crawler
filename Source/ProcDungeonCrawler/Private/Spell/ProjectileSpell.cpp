// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/ProjectileSpell.h"

#include "Characters/Wizard/WizardCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileSpell::AProjectileSpell()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

void AProjectileSpell::CastSpell(AWizardCharacter* WizardCharacter)
{
	Super::CastSpell(WizardCharacter);

	const FVector ForwardVector = WizardCharacter->GetControlRotation().Vector();
	ProjectileMovementComponent->Velocity = ForwardVector * ProjectileMovementComponent->InitialSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Spell casted with velocity %s!"), *ProjectileMovementComponent->Velocity.ToString())
}
