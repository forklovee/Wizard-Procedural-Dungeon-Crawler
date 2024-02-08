// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Human.h"

#include "Components/Character/InventoryComponent.h"
#include "Components/Character/PawnStats.h"
#include "Components/Character/SpellbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapon.h"
#include "Items/Item.h"
#include "Items/Clothes/Clothes.h"
#include "Items/Consumables/ConsumableItem.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

AHuman::AHuman()
{
	bUseControllerRotationYaw = true;

	PhysicalAnimation = CreateDefaultSubobject<UPhysicalAnimationComponent>(FName("PhysicalAnimation"));
	
	// Spell cast enabler
	SpellBook = CreateDefaultSubobject<USpellBookComponent>(FName("SpellBook"));
	// Pawn Stats
	Stats = CreateDefaultSubobject<UPawnStats>(FName("PawnStats"));
	Inventory = CreateDefaultSubobject<UInventoryComponent>(FName("Bag"));
	
	PrimaryActorTick.bCanEverTick = true;
}

void AHuman::BeginPlay()
{
	Super::BeginPlay();

	// Initialize standing and save crouch state
	UnCrouch();
	bIsCrouching = false;
	
	MovementSpeed = WalkingSpeed;
	
	OnTakeAnyDamage.AddDynamic(Stats, &UPawnStats::TakeDamage);
	
	// SpellBook->OnValidRuneSequenceCasted.AddDynamic(this, &AHuman::PrepareSpell);
	SpellBook->OnSpellCasted.AddDynamic(Stats, &UPawnStats::UseMana);
}

void AHuman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Display, TEXT("NOSZ KURAWA"))
}

void AHuman::PrimaryAction()
{
	// No prepared spell.
	if (!bCanAttack || !bCanPerformNextAttack)
	{
		bIsAttacking = false;
		return;
	}

	bIsAttacking = true;
	AttackNumber++;
	if (OnWeaponAttack.IsBound())
	{
		OnWeaponAttack.Broadcast(AttackNumber);
	}

	if (AttackNumber >= MaxAttackNumber)
	{
		AttackNumber = 0;
		bIsAttacking = false;
	}
}

void AHuman::SecondaryAction()
{
	if (OnSecondWeaponAttack.IsBound())
	{
		OnSecondWeaponAttack.Broadcast();
	}
}

void AHuman::UseItem(TSubclassOf<AItem> ItemClass)
{
	UE_LOG(LogTemp, Display, TEXT("Dupa"))
	if (ItemClass == nullptr) return;
	UE_LOG(LogTemp, Display, TEXT("Dupa valid"))

	const FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 21372137.f;
	AActor* ItemActor = GetWorld()->SpawnActor(ItemClass, &SpawnLocation);
	if (AConsumableItem* ConsumableItem = Cast<AConsumableItem>(ItemActor))
	{
		ConsumableItem->Use(this);
		Inventory->RemoveItem(ItemClass, 1);
	}
	else
	{
		ItemActor->Destroy();
	}
	
}

void AHuman::Interact()
{
}

void AHuman::Grab()
{
	
}

void AHuman::Release()
{
	SetItemGrab(nullptr);
}

void AHuman::SetItemGrab(AItem* Item)
{
	
}

void AHuman::SetWeapon(AWeapon* NewWeapon)
{
	if (NewWeapon == nullptr)
	{
		return;
	}
	
	if (Weapon.IsValid())
	{
		Weapon->UnEquip();
		Weapon = nullptr;
	}
	
	Weapon = NewWeapon;
	Weapon->Equip(this, GetMesh(), FName("WeaponSocket"));
}

void AHuman::SetArmor(AClothes* NewClothes, EArmorTarget ArmorTarget)
{
	switch (ArmorTarget)
	{
		default:
		case EArmorTarget::Head:
			HeadArmor = NewClothes;
			break;
		case EArmorTarget::Chest:
			ChestArmor = NewClothes;
			break;
		case EArmorTarget::Feet:
			FeetArmor = NewClothes;
			break;
	}
}

void AHuman::ResetCanAttack()
{
	bCanAttack = true;
	bCanPerformNextAttack = true;
	AttackNumber = 0;
	UE_LOG(LogTemp, Display, TEXT("Attack reset"))
}

// Movement
void AHuman::MoveAround(const FVector2D& MoveOffset)
{
	AddMovementInput(GetActorRightVector(), MoveOffset.Y);
	AddMovementInput(GetActorForwardVector(), MoveOffset.X);
}

void AHuman::LookAround(const FVector2D& LookOffset)
{
	AddControllerPitchInput(-LookOffset.Y);
	AddControllerYawInput(LookOffset.X);
}

void AHuman::Jump()
{
	Super::Jump();

	UE_LOG(LogTemp, Display, TEXT("Kurwa"))
}

void AHuman::SetSprinting(const bool bNewIsSprinting)
{
	bIsSprinting = bNewIsSprinting;
	
	MovementSpeed = bIsSprinting ? RunningSpeed : WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AHuman::ToggleCrouch()
{
	bIsCrouching = !bIsCrouching;
	if (OnCrouchStateChanged.IsBound())
	{
		OnCrouchStateChanged.Broadcast(bIsCrouching);
	}
}