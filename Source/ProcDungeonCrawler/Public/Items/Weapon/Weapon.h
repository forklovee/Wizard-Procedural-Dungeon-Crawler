// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class AHuman;
class UStatusEffect;

UCLASS()
class PROCDUNGEONCRAWLER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	
	UFUNCTION()
	void SetWeaponOwner(AHuman* WizardCharacter);
	
	UFUNCTION()
	void SetCanDealDamage(bool bNewCanDealDamage);

protected:
	virtual void BeginPlay() override;

	virtual void OnWeaponMeshHitPawn(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void DealDamage(APawn* DamagedPawn);

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"), Category="Weapon")
	USkeletalMeshComponent* WeaponMesh;
	
private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float Damage = 10.f;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	TArray<UStatusEffect*> OnPawnHitStatusEffects;
	
	TWeakObjectPtr<AHuman> WeaponOwner;
	bool bCanDealDamage = false;

	bool bItemMeshComponentHitConnected = false;
};
