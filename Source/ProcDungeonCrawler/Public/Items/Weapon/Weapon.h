// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItem.h"
#include "Weapon.generated.h"


class UStatusEffect;
class AWizardCharacter;

UCLASS()
class PROCDUNGEONCRAWLER_API AWeapon : public APickupItem
{
	GENERATED_BODY()

public:
	AWeapon();
	
	UFUNCTION()
	void SetAsPawnWeapon(AWizardCharacter* WizardCharacter);
	
	UFUNCTION()
	void SetCanDealDamage(bool bNewCanDealDamage);

	virtual TSubclassOf<AActor> Pickup_Implementation(APawn* Pawn) override;
	virtual void Grab_Implementation() override;
protected:
	virtual void BeginPlay() override;

	virtual void OnWeaponMeshHitPawn(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void DealDamage(APawn* DamagedPawn);

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	float Damage = 10.f;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess="true"))
	TArray<UStatusEffect*> OnPawnHitStatusEffects;
	
	TWeakObjectPtr<AWizardCharacter> WeaponOwner;
	bool bCanDealDamage = false;

	bool bItemMeshComponentHitConnected = false;
};
