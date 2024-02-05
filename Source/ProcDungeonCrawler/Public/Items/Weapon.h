// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "Weapon.generated.h"

class AHuman;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipped);

UCLASS()
class PROCDUNGEONCRAWLER_API AWeapon : public APickupItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttack OnWeaponEquipped;
	UPROPERTY(BlueprintAssignable)
	FOnAttack OnWeaponUnequipped;
	
	AWeapon();

	/* Only broadcasts OnAttack delegate */
	void Attack();

	virtual void Equip(AHuman* HumanPawn, USceneComponent* EquipTargetComponent, FName SocketName = "") override;
	virtual void UnEquip() override;
	
protected:
	bool CanAttack() const { return bCanAttack; }

private:
	/* Resets bCanAttack to True */
	void ResetCanAttack();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	USceneComponent* WeaponHandlePoint;
	
protected:
	/* Damage dealt by this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	float Length = 55.f;
	
	/* Attack delay in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	float AttackDelay = 0.5;

private:
	TWeakObjectPtr<AHuman> WeaponOwner;
	
	/* Whether this weapon can attack */
	bool bCanAttack = false;

	/* Timer handle to reset bCanAttack to True */
	FTimerHandle AttackTimerHandle;
};
