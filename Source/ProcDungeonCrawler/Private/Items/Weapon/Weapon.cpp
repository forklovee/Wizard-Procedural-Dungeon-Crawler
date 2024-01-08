// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"

#include "Characters/Human/Human.h"
#include "Engine/DamageEvents.h"

AWeapon::AWeapon(): Super()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent->SetupAttachment(WeaponMesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void AWeapon::SetWeaponOwner(AHuman* WizardCharacter)
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

void AWeapon::DealDamage(APawn* DamagedPawn)
{
	if (!bCanDealDamage || WeaponOwner == nullptr || DamagedPawn == nullptr) return;
	
	DamagedPawn->TakeDamage(Damage, FDamageEvent(), WeaponOwner->GetController(), WeaponOwner.Get());
}

void AWeapon::OnWeaponMeshHitPawn(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
}