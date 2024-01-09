// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

class UPlayerHUD;

enum class EInputContextType : uint8
{
	Movement = 0,
	Interaction = 1,
	UI = 2,
};

class AWeapon;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLookAroundInput, const FVector2D&, LookOffset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveAroundInput, const FVector2D&, MoveOffset);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprintToggled, bool, bIsSprinting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrouchToggled, bool, bIsCrouching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpActionInput);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrimaryHandActionInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractActionInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbedActionInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleasedActionInput);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUILeftRightInput, int, Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToggleBagRequest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneSlotSelected, int, RuneIdx);

UCLASS()
class PROCDUNGEONCRAWLER_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Movement
	UPROPERTY(BlueprintAssignable)
	FOnLookAroundInput OnMoveAroundInput;
	UPROPERTY(BlueprintAssignable)
	FOnLookAroundInput OnLookAroundInput;
	
	UPROPERTY(BlueprintAssignable)
	FOnSprintToggled OnSprintToggled;
	UPROPERTY(BlueprintAssignable)
	FOnCrouchToggled OnCrouchToggled;
	UPROPERTY(BlueprintAssignable)
	FOnJumpActionInput OnJumpActionInput;

	// Interaction
	UPROPERTY(BlueprintAssignable)
	FOnPrimaryHandActionInput OnPrimaryHandActionInput;
	UPROPERTY(BlueprintAssignable)
	FOnInteractActionInput OnInteractActionInput;
	UPROPERTY(BlueprintAssignable)
	FOnGrabbedActionInput OnGrabbedActionInput;
	UPROPERTY(BlueprintAssignable)
	FOnReleasedActionInput OnReleasedActionInput;
	
	UPROPERTY(BlueprintAssignable)
	FOnUILeftRightInput OnUILeftRightInput;

	UPROPERTY(BlueprintAssignable)
	FOnToggleBagRequest OnToggleBagRequest;
	UPROPERTY(BlueprintAssignable)
	FOnRuneSlotSelected OnRuneSlotSelected;

	void SetupDefaultInput(UEnhancedInputComponent* PlayerInputComponent);
	void SetInputContext(EInputContextType InputContext, bool bState = true);
	
	// Input Context Enablers
	// UFUNCTION()
	// void UpdateSpellBookInputContext(bool IsSpellBookOpen);
	// UFUNCTION()
	// void UpdateMapInputContext(bool IsMapOpenOpen);
	
	UPlayerHUD* SetupWizardHud();
	UPlayerHUD* GetWizardHud() const { return WizardHUD; }
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
private:
	void SetInteractionInput(bool bState) const;
	void SetCharacterMovementInput(bool bState) const;

	// Movement Input
	UFUNCTION()
	void OnLookAroundInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnMoveAroundInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnSetSprintInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnJumpInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnSetCrouchInputAction(const FInputActionValue& Value);
	
	// Interaction Input
	UFUNCTION()
	void OnUseWeaponInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnCastPreparedSpellInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnUseItemInHandInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnCastRuneFromSlotInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnInteractionInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnGrabItemInputAction(const FInputActionValue& Value);
	
	// UI Input
	UFUNCTION()
	void OnToggleInventoryInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnToggleSpellBookInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnTogglePauseMenuInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnToggleMapInputAction(const FInputActionValue& Value);

public:
	// Movement Input
	UPROPERTY(EditAnywhere, Category= "Input|Movement")
	TSoftObjectPtr<UInputMappingContext> Movement_InputContext;
	
	UPROPERTY(EditAnywhere, Category= "Input|Movement")
	TSoftObjectPtr<UInputAction> MoveAround_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|Movement")
	TSoftObjectPtr<UInputAction> LookAround_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Movement")
	TSoftObjectPtr<UInputAction> Jump_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Movement")
	TSoftObjectPtr<UInputAction> Crouch_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|Movement")
	TSoftObjectPtr<UInputAction> Sprint_InputAction;

	// Interaction Input
	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputMappingContext> Interaction_InputContext;
	
	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> UseWeapon_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> CastPreparedSpell_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> UseItemInHand_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> CastRuneFromSlot_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> Interaction_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> GrabItem_InputAction;

	// UI Input
	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputMappingContext> UI_InputContext;
	
	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> ToggleInventory_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> ToggleSpellBook_InputAction;
	
	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> TogglePauseMenu_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|UI")
	TSoftObjectPtr<UInputAction> ToggleMap_InputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UPlayerHUD> WizardHUDClass;

	UPROPERTY(EditAnywhere)
	UPlayerHUD* WizardHUD;

private:
	TSubclassOf<AWeapon> CurrentWeaponClass;

	bool bMovementLocked = false;
	bool bLookAroundLocked = false;
	bool bInteractionLocked = false;
};
