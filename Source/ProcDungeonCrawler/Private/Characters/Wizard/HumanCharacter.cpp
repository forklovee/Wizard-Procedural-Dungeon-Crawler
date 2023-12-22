// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/Player/WizardPlayerController.h"
#include "Characters/Wizard/WizardCharacter.h"

#include "Components/Character/PawnStats.h"
#include "Components/Character/SpellbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AWizardCharacter::AWizardCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	bUseControllerRotationYaw = true;

	// Hand sockets
	HandsRootComponent = CreateDefaultSubobject<USceneComponent>(FName("HandsRoot"));
	HandsRootComponent->SetupAttachment(RootComponent);
	
	LeftHandSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("LeftHandSocket"));
	LeftHandSocketComponent->SetupAttachment(HandsRootComponent, NAME_None);
	RightHandSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("RightHandSocket"));
	RightHandSocketComponent->SetupAttachment(HandsRootComponent, NAME_None);

	// Spell cast enabler
	SpellBook = CreateDefaultSubobject<USpellbookComponent>(FName("Spellbook"));
	SpellBook->SetupAttachment(LeftHandSocketComponent);

	// Pawn Stats
	WizardStats = CreateDefaultSubobject<UPawnStats>(FName("WizardStats"));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementSpeed = WalkingSpeed;
	
	OnTakeAnyDamage.AddDynamic(WizardStats, &UPawnStats::TakeDamage);

	SpellBook->OnValidRuneSequenceCasted.AddDynamic(this, &AWizardCharacter::PrepareSpell);
	SpellBook->OnSpellCasted.AddDynamic(WizardStats, &UPawnStats::UseMana);

	if (AWizardPlayerController* PlayerController = Cast<AWizardPlayerController>(GetController()))
	{
		PlayerController->OnMoveAroundInput.AddDynamic(this, &AWizardCharacter::MoveAround);
		PlayerController->OnLookAroundInput.AddDynamic(this, &AWizardCharacter::LookAround);

		PlayerController->OnSprintToggled.AddDynamic(this, &AWizardCharacter::SetSprinting);
		PlayerController->OnCrouchToggled.AddDynamic(this, &AWizardCharacter::SetCrouch);
		PlayerController->OnJumpActionInput.AddDynamic(this, &AWizardCharacter::Jump);

		PlayerController->OnPrimaryHandActionInput.AddDynamic(this, &AWizardCharacter::UseWeapon);
		PlayerController->OnInteractActionInput.AddDynamic(this, &AWizardCharacter::Interact);
		
		PlayerController->OnRuneSlotSelected.AddDynamic(SpellBook, &USpellbookComponent::CastRuneOfIdx);
	}
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

	AddControllerPitchInput(LookOffset.Y);
	AddControllerYawInput(LookOffset.X);
}

void AWizardCharacter::SetSprinting(const bool bNewIsSprinting)
{
	bIsSprinting = bNewIsSprinting;
	
	MovementSpeed = bIsSprinting ? RunningSpeed : WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AWizardCharacter::SetCrouch(const bool bNewIsCrouching)
{
	bIsCrouching = bNewIsCrouching;
	if (bNewIsCrouching)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AWizardCharacter::SetCombatMode(bool bNewInCombatMode)
{
	bInCombatMode = bNewInCombatMode;
}

// Hand action and Interaction
void AWizardCharacter::PrimaryHandAction()
{
	// No prepared spell.
	if (!SpellBook->IsSpellPrepared()) return;
	
	SpellBook->CastPreparedSpell(this);
}

void AWizardCharacter::PrepareSpell(TSubclassOf<ASpellCast> SpellCastClass, float ManaCost)
{
	if (WizardStats->HasRequiredMana(ManaCost))
	{
		SpellBook->PrepareSpell(SpellCastClass, ManaCost);
	}
	else
	{
		SpellBook->ClearCastedRunes();
	}
}
