// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Human.generated.h"

class UInventoryComponent;
class USpellBookComponent;
class ASpellCast;
class UPawnStats;
class AItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAttack);

UCLASS(Blueprintable, BlueprintType)
class PROCDUNGEONCRAWLER_API AHuman : public ACharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnWeaponAttack OnWeaponAttack;
	
	AHuman();
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	
	// Interaction
	UFUNCTION()
	void UseWeapon();
	UFUNCTION()
	void UseItem(TSubclassOf<AItem> Item, int ItemAmount);
	UFUNCTION()
	void Interact();

	UFUNCTION()
	void Grab(TWeakObjectPtr<AItem> PickupItem = nullptr);
	UFUNCTION()
	void Release();
	
	// Movement
	UFUNCTION(BlueprintCallable)
	void MoveAround(const FVector2D& MoveOffset);
	UFUNCTION(BlueprintCallable)
	void LookAround(const FVector2D& LookOffset);
	UFUNCTION()
	void SetSprinting(const bool bNewIsSprinting);
	
	bool IsCrouching() const { return bIsCrouching; }
	UFUNCTION()
	void SetCrouch(const bool bNewIsCrounching);

	UFUNCTION()
	void SetCombatMode(bool bNewInCombatMode);

private:
	UFUNCTION()
	void PrepareSpell(TSubclassOf<ASpell> SpellCastClass, float ManaCost);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USpellBookComponent* SpellBook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UInventoryComponent* Bag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UPawnStats* Stats;
	
private:
	bool bInCombatMode = false;
	
	bool bIsCrouching = false;
	bool bBlockMovement = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 400.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 700.0f;

	float MovementSpeed = 0.0f;
};


