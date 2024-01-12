// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class AHuman;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipped);

UCLASS()
class PROCDUNGEONCRAWLER_API AWeapon : public AItem
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

	void Equip(AHuman* HumanPawn, USceneComponent* EquipTargetComponent, FName SocketName = "");
	void UnEquip();
	
protected:
	bool CanAttack() const { return bCanAttack; }

private:
	/* Resets bCanAttack to True */
	void ResetCanAttack();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	USkeletalMeshComponent* WeaponMesh;
	
protected:
	/* Damage dealt by this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	float Damage = 10.f;

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
