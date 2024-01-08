// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

class UPlayerHUD;

enum class EInputContextType : uint8
{
	None = 0,
	Gameplay = 1 << 0,
	UI = 1 << 1,
	GameplayAndUI = Gameplay | UI
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

	// Interaction Input
	UFUNCTION()
	void OnPrimaryHandAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnInteractAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnGrabItemAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnReleaseItemAction(const FInputActionValue& Value);

	// Movement Input
	UFUNCTION()
	void OnMoveAroundAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnLookAroundAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnSetSprintAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnSetCrouchAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnJumpAction(const FInputActionValue& Value);
	
	// Bag Input
	UFUNCTION()
	void OnToggleBagAction(const FInputActionValue& Value);

	// SpellBook Input
	UFUNCTION()
	void OnRuneSlotKeyPressed(const FInputActionValue& Value);

public:
	// Input
	UPROPERTY(EditAnywhere, Category= "Input")
	TSoftObjectPtr<UInputMappingContext> Gameplay_InputContext;
	UPROPERTY(EditAnywhere, Category= "Input")
	TSoftObjectPtr<UInputMappingContext> UI_InputContext;
	
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
	TSoftObjectPtr<UInputAction> GrabItemBegin_InputAction;
	UPROPERTY(EditAnywhere, Category= "Input|Actions|Interaction")
	TSoftObjectPtr<UInputAction> GrabItemEnd_InputAction;
	
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
