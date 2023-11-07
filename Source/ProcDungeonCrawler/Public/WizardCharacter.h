// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "WizardCharacter.generated.h"

class USpellbookComponent;
class ABagActor;
class UWidgetInteractionComponent;
class UBagComponent;
class UInputMappingContext;
class UInputAction;

class ASpellCast;
class URuneCast;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractionTarget, FName, ItemName, FName, InteractionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleBagRequest, bool, bIsOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneSlotSelected, int, RuneIdx);

UCLASS()
class PROCDUNGEONCRAWLER_API AWizardCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionTarget OnNewInteractionTarget;
	UPROPERTY(BlueprintAssignable)
	FOnToggleBagRequest OnToggleBagRequest;
	UPROPERTY(BlueprintAssignable)
	FOnRuneSlotSelected OnRuneSlotSelected;

	AWizardCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	// Input
	void UpdateInputContexts(const struct FInputActionValue& Value);
	void OnRuneSlotKeyPressed(const FInputActionValue& Value);
	void SetInteractionInput(bool bState) const;
	void SetCharacterMovementInput(bool bState) const;
	
	// Movement and Looking around
	void MoveAround(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void ToggleSprint(const FInputActionValue& Value);
	
	// Interaction
	void PrimaryAction(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);

	// Bag
	void ToggleBagAction(const FInputActionValue& Value);
	
public:
	// Components
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ArmsMeshComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* RightHandSocketComponent;
	UPROPERTY(EditAnywhere)
	USceneComponent* LeftHandSocketComponent;

	UPROPERTY(EditAnywhere)
	UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	USceneComponent* BagSocket;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	UBagComponent* Bag;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	USpellbookComponent* SpellBook;
	
	// Input
	UPROPERTY(EditAnywhere, Category= "Input")
	TSoftObjectPtr<UInputMappingContext> MouseMoveAndUI_Context;
	UPROPERTY(EditAnywhere, Category= "Input")
	TSoftObjectPtr<UInputMappingContext> CharacterMovement_InputContext;
	UPROPERTY(EditAnywhere, Category= "Input")
	TSoftObjectPtr<UInputMappingContext> Interaction_InputContext;

	
	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> LookAround_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Movement_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Jump_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Sprint_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|Actions|Common")
	TSoftObjectPtr<UInputAction> Crouch_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Interaction")
	TSoftObjectPtr<UInputAction> Interaction_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Interaction")
	TSoftObjectPtr<UInputAction> PrimaryAction_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Magic")
	TSoftObjectPtr<UInputAction> RuneCast_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Actions|Magic")
	TSoftObjectPtr<UInputAction> ChangeSpellSlot_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> OpenBag_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> OpenMap_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> OpenSpellBook_InputAction;

private:
	FHitResult InteractionTarget;

	bool bBlockMovement = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 14.0f;

	float MovementSpeed = 0.0f;
};
