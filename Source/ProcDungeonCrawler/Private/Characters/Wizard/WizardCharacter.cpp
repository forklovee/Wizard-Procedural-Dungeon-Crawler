// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Wizard/WizardCharacter.h"

#include "Components/Character/SpellbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Interface/PropPickupInterface.h"
#include "Spell/RuneCast.h"

AWizardCharacter::AWizardCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	bUseControllerRotationYaw = true;

	// Hand sockets
	HeadSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("HeadSocket"));
	HeadSocketComponent->SetupAttachment(RootComponent, FName("Head"));
	LeftHandSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("LeftHandSocket"));
	LeftHandSocketComponent->SetupAttachment(HeadSocketComponent, FName("LeftHand"));
	RightHandSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("RightHandSocket"));
	RightHandSocketComponent->SetupAttachment(HeadSocketComponent, FName("RightHand"));
	
	SpellBook = CreateDefaultSubobject<USpellbookComponent>(FName("Spellbook"));
	SpellBook->SetupAttachment(LeftHandSocketComponent);
	
	PrimaryActorTick.bCanEverTick = true;
}

void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementSpeed = WalkingSpeed;
}

// Movement
void AWizardCharacter::MoveAround(const FVector2D& MoveOffset)
{
	if (Controller == nullptr) return;
	
	AddMovementInput(GetActorRightVector(), MoveOffset.Y);
	AddMovementInput(GetActorForwardVector(), MoveOffset.X);
}

void AWizardCharacter::LookAround(const FVector2D& LookOffset)
{
	if (Controller == nullptr) return;

	AddControllerYawInput(LookOffset.X);
	AddControllerPitchInput(-LookOffset.Y);
}

void AWizardCharacter::SetSprinting(const bool bNewIsSprinting)
{
	bIsSprinting = bNewIsSprinting;
	
	MovementSpeed = bIsSprinting ? RunningSpeed : WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

// Hand action and Interaction
void AWizardCharacter::PrimaryHandAction()
{
	// No prepared spell.
	if (!SpellBook->IsSpellPrepared()) return;
	
	SpellBook->CastPreparedSpell(this);
}

AActor* AWizardCharacter::Interact(AActor* Actor)
{
	if (Actor == nullptr ||
		!Actor->Implements<UPropPickupInterface>() //TODO: Add Interaction Interface Check
		)
	{
		return nullptr;
	}

	UDataAsset* AdditionalDataAsset = IPropPickupInterface::Execute_GetAdditionalDataAsset(Actor);

	// Add rune to Spell Book
	if (URuneCast* RuneCast = Cast<URuneCast>(AdditionalDataAsset))
	{
		SpellBook->AddRune(RuneCast);
		IPropPickupInterface::Execute_Pickup(Actor, this);
	}
	
	return Actor;
}

AActor* AWizardCharacter::HoldItem(AActor* Actor)
{
	if (Actor == nullptr ||
		!Actor->Implements<UPropPickupInterface>() //TODO: Add Interaction Interface Check
		)
	{
		return nullptr;
	}
	
	return Actor;
}

