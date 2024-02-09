// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Human.generated.h"

class UPhysicalAnimationComponent;

UENUM(Blueprintable, BlueprintType)
enum class EArmorTarget: uint8
{
	Head,
	Chest,
	Feet,
	Hands
};

class AClothes;
class AWeapon;
class UInventoryComponent;
class USpellBookComponent;
class ASpellCast;
class UPawnStats;
class AItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAttack, int, AttackNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecondWeaponAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrouchStateChanged, bool, bCrouchState);

UCLASS(Blueprintable, BlueprintType)
class PROCDUNGEONCRAWLER_API AHuman : public ACharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttack OnWeaponAttack;
	UPROPERTY(BlueprintAssignable)
	FOnSecondWeaponAttack OnSecondWeaponAttack;
	
	UPROPERTY(BlueprintAssignable)
	FOnCrouchStateChanged OnCrouchStateChanged;
	
	AHuman();
	
	UFUNCTION()
	void UseItem(TSubclassOf<AItem> ItemClass);
	UFUNCTION()
	void DropItem(TSubclassOf<AItem> ItemClass);

	// Movement
	UFUNCTION(BlueprintCallable)
	bool IsSprinting() const { return bIsSprinting; }
	UFUNCTION(BlueprintCallable)
	bool IsCrouching() const { return bIsCrouching; }
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Movement
	UFUNCTION()
	void MoveAround(const FVector2D& MoveOffset);
	UFUNCTION()
	void LookAround(const FVector2D& LookOffset);
	
	void Jump() override;
	
	UFUNCTION()
	void SetSprinting(const bool bNewIsSprinting);
	UFUNCTION()
	void ToggleCrouch();
	
	// Interaction
	UFUNCTION()
	void PrimaryAction();
	UFUNCTION()
	void SecondaryAction();
	
	UFUNCTION()
	virtual void Interact();
	UFUNCTION()
	void Grab();
	UFUNCTION()
	void Release();

	virtual void SetItemGrab(AItem* NewGrabItem);
	UFUNCTION()
	virtual void SetWeapon(AWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeapon() const { return Weapon.Get(); }
	
	UFUNCTION()
	virtual void SetArmor(AClothes* NewClothes, EArmorTarget ArmorTarget);

	void ResetCanAttack();
// private:
	// UFUNCTION()
	// void PrepareSpell(TSubclassOf<ASpell> SpellCastClass, float ManaCost);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	UPhysicalAnimationComponent* PhysicalAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells")
	USpellBookComponent* SpellBook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Human")
	UInventoryComponent* Inventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Human")
	UPawnStats* Stats;

protected:
	// Equipment
	TWeakObjectPtr<AItem> GrabbedItem;
	TWeakObjectPtr<AWeapon> Weapon;

	TWeakObjectPtr<AClothes> HeadArmor;
	TWeakObjectPtr<AClothes> ChestArmor;
	TWeakObjectPtr<AClothes> FeetArmor;
	TArray<TWeakObjectPtr<AClothes>> Rings;

	// Combat
	// Animation controlled Attack Lock
	UPROPERTY(BlueprintReadWrite, Category= "Combat")
	bool bCanPerformNextAttack = true;
	
private:
	// Combat
	UPROPERTY(EditAnywhere, Category= "Combat", meta = (AllowPrivateAccess = "true"))
	int AttackDelay = 0.15;
	UPROPERTY(EditAnywhere, Category= "Combat", meta = (AllowPrivateAccess = "true"))
	int MaxAttackNumber = 3;
	int AttackNumber = 0;

	bool bIsAttacking = false;
	// General usage Attack Lock
	bool bCanAttack = true;

	FTimerHandle AttackDelayTimerHandle;

	// Movement
	bool bIsCrouching = false;
	bool bBlockMovement = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 400.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 700.0f;

	float MovementSpeed = 0.0f;
};


