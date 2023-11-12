// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "WizardCharacter.generated.h"

class APickupItem;
class USpellbookComponent;

UCLASS(Blueprintable, BlueprintType)
class PROCDUNGEONCRAWLER_API AWizardCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	AWizardCharacter();
	
protected:
	virtual void BeginPlay() override;
	
	// Interaction
	void PrimaryHandAction();

	// Movement
	void MoveAround(const FVector2D& MoveOffset);
	void LookAround(const FVector2D& LookOffset);
	void SetSprinting(const bool bNewIsSprinting);

	bool IsCrouching() const { return bIsCrouching; }
	void SetCrouch(const bool bNewIsCrounching);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	USpellbookComponent* SpellBook;

	UPROPERTY(EditAnywhere)
	USceneComponent* HandsRootComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* RightHandSocketComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* LeftHandSocketComponent;
private:
	bool bIsCrouching = false;
	bool bBlockMovement = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 14.0f;

	float MovementSpeed = 0.0f;
};
