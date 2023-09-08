// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Spell\RuneCast.h"

#include "WizardCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneAdded, URuneCast*, RuneCast);

UCLASS()
class PROCDUNGEONCRAWLER_API AWizardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	FOnRuneAdded OnRuneAdded;
	
	AWizardCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	bool AddRune(TSoftObjectPtr<URuneCast> NewRuneCast);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	// Movement and Looking around
	void MoveAround(const struct FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void ToggleSprint(const FInputActionValue& Value);

	// Magic
	void CastRune(const FInputActionValue& Value);
	void CastSpell();

	void ClearCastedRunes();
	
	// Interaction
	void PrimaryAction(const FInputActionValue& Value);
	
public:
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ArmsMeshComponent;

	UPROPERTY(EditAnywhere, Category= "Input")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> LookAround_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Movement_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Jump_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Sprint_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Interaction")
	TSoftObjectPtr<UInputAction> Interaction_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Interaction")
	TSoftObjectPtr<UInputAction> PrimaryAction_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Magic")
	TSoftObjectPtr<UInputAction> RuneCast_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Magic")
	TSoftObjectPtr<UInputAction> ChangeSpellSlot_InputAction;
	
private:
	TArray<FHitResult> LookingAt_HitActors;

	TArray<URuneCast*> RuneCasts;
	
	TArray<URuneCast*> CastedRunes;
	FTimerHandle ClearCastedRunesTimerHandle;
	
	bool bIsCastingSpell = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 14.0f;

	float MovementSpeed = 0.0f;
};
