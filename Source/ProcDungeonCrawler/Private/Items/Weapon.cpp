// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"

#include "Characters/Human/Human.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponHandlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponHandlePoint"));
	WeaponHandlePoint->SetupAttachment(RootComponent);
}

void AWeapon::Attack()
{
	if (!CanAttack()) return;
	
	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AWeapon::ResetCanAttack, AttackDelay, false);
	
	if (OnAttack.IsBound())
	{
		OnAttack.Broadcast();
	}
}

void AWeapon::Equip(AHuman* HumanPawn, USceneComponent* EquipTargetComponent, FName SocketName)
{
	if (HumanPawn == nullptr)
	{
		return;
	}

	WeaponOwner = HumanPawn;

	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(EquipTargetComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

	if (OnWeaponEquipped.IsBound())
	{
		OnWeaponEquipped.Broadcast();
	}
}

void AWeapon::UnEquip()
{
	if (!WeaponOwner.IsValid())
	{
		return;
	}
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponOwner = nullptr;
	
	if (OnWeaponUnequipped.IsBound())
	{
		OnWeaponUnequipped.Broadcast();
	}
}

void AWeapon::ResetCanAttack()
{
	bCanAttack = true;
}
