// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Human.h"

#include "Characters/Human/Player/DefaultPlayerController.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/PawnStats.h"
#include "Components/Character/SpellbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AHuman::AHuman()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	bUseControllerRotationYaw = true;
	
	// Spell cast enabler
	SpellBook = CreateDefaultSubobject<USpellBookComponent>(FName("SpellBook"));

	// Pawn Stats
	Stats = CreateDefaultSubobject<UPawnStats>(FName("WizardStats"));

	Bag = CreateDefaultSubobject<UInventoryComponent>(FName("Bag"));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AHuman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AHuman::BeginPlay()
{
	Super::BeginPlay();

	MovementSpeed = WalkingSpeed;
	
	OnTakeAnyDamage.AddDynamic(Stats, &UPawnStats::TakeDamage);

	// SpellBook->OnValidRuneSequenceCasted.AddDynamic(this, &AHuman::PrepareSpell);
	SpellBook->OnSpellCasted.AddDynamic(Stats, &UPawnStats::UseMana);

	if (ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(GetController()))
	{
		PlayerController->OnMoveAroundInput.AddDynamic(this, &AHuman::MoveAround);
		PlayerController->OnLookAroundInput.AddDynamic(this, &AHuman::LookAround);

		PlayerController->OnSprintToggled.AddDynamic(this, &AHuman::SetSprinting);
		PlayerController->OnCrouchToggled.AddDynamic(this, &AHuman::SetCrouch);
		PlayerController->OnJumpActionInput.AddDynamic(this, &AHuman::Jump);

		PlayerController->OnPrimaryHandActionInput.AddDynamic(this, &AHuman::UseWeapon);
		PlayerController->OnInteractActionInput.AddDynamic(this, &AHuman::Interact);
		PlayerController->OnRuneSlotSelected.AddDynamic(SpellBook, &USpellBookComponent::CastRuneOfIdx);
	}
}

void AHuman::UseWeapon()
{
	UE_LOG(LogTemp, Display, TEXT("chujDupa"));

	// No prepared spell.
	if (!SpellBook->IsSpellPrepared())
	{
		if (OnWeaponAttack.IsBound())
		{
			OnWeaponAttack.Broadcast();
		}
		return;
	}
	
	// SpellBook->CastPreparedSpell(this);
}

void AHuman::UseItem(TSubclassOf<AItem> Item, int ItemAmount)
{
	
}

void AHuman::Interact()
{
	
}

void AHuman::Grab(TWeakObjectPtr<AItem> PickupItem)
{
	
}

void AHuman::Release()
{
}

// Movement
void AHuman::MoveAround(const FVector2D& MoveOffset)
{
	if (Controller == nullptr) return;
	
	AddMovementInput(GetActorRightVector(), MoveOffset.Y);
	AddMovementInput(GetActorForwardVector(), MoveOffset.X);
}

void AHuman::LookAround(const FVector2D& LookOffset)
{
	if (Controller == nullptr) return;

	AddControllerPitchInput(-LookOffset.Y);
	AddControllerYawInput(LookOffset.X);
}

void AHuman::SetSprinting(const bool bNewIsSprinting)
{
	bIsSprinting = bNewIsSprinting;
	
	MovementSpeed = bIsSprinting ? RunningSpeed : WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AHuman::SetCrouch(const bool bNewIsCrouching)
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

void AHuman::SetCombatMode(bool bNewInCombatMode)
{
	bInCombatMode = bNewInCombatMode;
}

void AHuman::PrepareSpell(TSubclassOf<ASpell> SpellCastClass, float ManaCost)
{
	// if (Stats->HasRequiredMana(ManaCost))
	// {
	// 	SpellBook->PrepareSpell(SpellCastClass, ManaCost);
	// }
	// else
	// {
	// 	SpellBook->ClearCastedRunes();
	// }
}
