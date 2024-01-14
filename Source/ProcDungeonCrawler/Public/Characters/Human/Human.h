// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Human.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrouchStateChanged, bool, bCrouchState);

UCLASS(Blueprintable, BlueprintType)
class PROCDUNGEONCRAWLER_API AHuman : public ACharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttack OnWeaponAttack;

	UPROPERTY(BlueprintAssignable)
	FOnCrouchStateChanged OnCrouchStateChanged;
	
	AHuman();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(AWeapon* NewWeapon, USceneComponent* EquipTargetComponent, FName SocketName = "");
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

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
	void UseItem(TSubclassOf<AItem> Item, int ItemAmount);
	UFUNCTION()
	virtual void Interact();
	UFUNCTION()
	void Grab();
	UFUNCTION()
	void Release();

	virtual void SetItemGrab(AItem* NewGrabItem);
	virtual void SetWeapon(AWeapon* NewWeapon);
	virtual void SetArmor(AClothes* NewClothes, EArmorTarget ArmorTarget);

// private:
	// UFUNCTION()
	// void PrepareSpell(TSubclassOf<ASpell> SpellCastClass, float ManaCost);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USpellBookComponent* SpellBook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UInventoryComponent* Bag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UPawnStats* Stats;

protected:
	TWeakObjectPtr<AItem> GrabbedItem;
	TWeakObjectPtr<AWeapon> Weapon;

	TWeakObjectPtr<AClothes> HeadArmor;
	TWeakObjectPtr<AClothes> ChestArmor;
	TWeakObjectPtr<AClothes> FeetArmor;
	TArray<TWeakObjectPtr<AClothes>> Rings;

private:
	bool bIsCrouching = false;
	bool bBlockMovement = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 400.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 700.0f;

	float MovementSpeed = 0.0f;
};


