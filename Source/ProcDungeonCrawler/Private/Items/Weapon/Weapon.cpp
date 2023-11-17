// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"
#include "Characters/Wizard/WizardCharacter.h"

AWeapon::AWeapon(): Super()
{
	
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void AWeapon::SetAsPawnWeapon(AWizardCharacter* WizardCharacter)
{
	WeaponOwner = WizardCharacter;

	if (!bItemMeshComponentHitConnected && WeaponOwner.IsValid())
	{
		ItemMesh->OnComponentHit.AddDynamic(this, &AWeapon::OnWeaponMeshHitPawn);
		bItemMeshComponentHitConnected = true;
	}
	if (bItemMeshComponentHitConnected && !WeaponOwner.IsValid())
	{
		ItemMesh->OnComponentHit.RemoveDynamic(this, &AWeapon::OnWeaponMeshHitPawn);
		bItemMeshComponentHitConnected = false;
	}
}

void AWeapon::SetCanDealDamage(bool bNewCanDealDamage)
{
	bCanDealDamage = bNewCanDealDamage;
	const FName DamageDealerTag = FName("DamageDealer");

	if (bCanDealDamage && !Tags.Contains(DamageDealerTag))
	{
		Tags.Add(DamageDealerTag);
	}
	else if (!bCanDealDamage && Tags.Contains(DamageDealerTag))
	{
		Tags.Remove(DamageDealerTag);
	}
}

TSubclassOf<AActor> AWeapon::Pickup_Implementation(APawn* Pawn)
{
	const TSubclassOf<AActor> ItemClass = GetClass();
	
	return ItemClass;
}

void AWeapon::Grab_Implementation()
{
	Super::Grab_Implementation();
}

void AWeapon::DealDamage(APawn* DamagedPawn)
{
	
}

void AWeapon::OnWeaponMeshHitPawn(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
}