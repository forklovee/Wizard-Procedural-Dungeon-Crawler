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

// Movement
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLookAroundAction, const FVector2D&, LookOffset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveAroundAction, const FVector2D&, MoveOffset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSprintToggledAction, bool, bIsPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchToggledAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpActionInput);

// Interaction
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrimaryAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneSlotSelectedAction, const int&, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbedAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleasedAction);

// UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToggleInventoryAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleSpellBookAction, bool, bIsPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTogglePauseMenuAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToggleMapAction);

UCLASS()
class PROCDUNGEONCRAWLER_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Movement
	UPROPERTY(BlueprintAssignable)
	FOnLookAroundAction OnLookAroundAction;
	UPROPERTY(BlueprintAssignable)
	FOnMoveAroundAction OnMoveAroundAction;
	UPROPERTY(BlueprintAssignable)
	FOnSprintToggledAction OnSprintToggledAction;
	UPROPERTY(BlueprintAssignable)
	FOnCrouchToggledAction OnCrouchToggledAction;
	UPROPERTY(BlueprintAssignable)
	FOnJumpActionInput OnJumpAction;

	// Interaction
	UPROPERTY(BlueprintAssignable)
	FOnPrimaryAction OnPrimaryAction;
	UPROPERTY(BlueprintAssignable)
	FOnRuneSlotSelectedAction OnRuneSlotSelectedAction;
	UPROPERTY(BlueprintAssignable)
	FOnInteractAction OnInteractAction;
	UPROPERTY(BlueprintAssignable)
	FOnGrabbedAction OnGrabbedAction;
	UPROPERTY(BlueprintAssignable)
	FOnReleasedAction OnReleasedAction;

	// UI
	UPROPERTY(BlueprintAssignable)
	FOnToggleInventoryAction OnToggleInventoryAction;
	UPROPERTY(BlueprintAssignable)
	FOnToggleSpellBookAction OnToggleSpellBookAction;
	UPROPERTY(BlueprintAssignable)
	FOnTogglePauseMenuAction OnTogglePauseMenuAction;
	UPROPERTY(BlueprintAssignable)
	FOnTogglePauseMenuAction OnToggleMapAction;

	void SetupDefaultInput(UEnhancedInputComponent* PlayerInputComponent);
	void SetInputContext(EInputContextType InputContext, bool bState = true);
	
	// Input Context Enablers
	// UFUNCTION()
	// void UpdateSpellBookInputContext(bool IsSpellBookOpen);
	// UFUNCTION()
	// void UpdateMapInputContext(bool IsMapOpenOpen);
	
	UPlayerHUD* AddHudToViewport();
	UPlayerHUD* GetHud() const { return WizardHUD; }
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
private:
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
	void OnPrimaryInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnRuneSlotSelectedInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnInteractionInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnGrabItemInputAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnReleasedItemInputAction(const FInputActionValue& Value);
	
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
	TSoftObjectPtr<UInputAction> PrimaryAction_InputAction;

	UPROPERTY(EditAnywhere, Category= "Input|Action")
	TSoftObjectPtr<UInputAction> OnRuneSlotSelected_InputAction;

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
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditAnywhere)
	UPlayerHUD* WizardHUD;

private:
	TSubclassOf<AWeapon> CurrentWeaponClass;

	bool bMovementLocked = false;
	bool bLookAroundLocked = false;
	bool bInteractionLocked = false;
};
