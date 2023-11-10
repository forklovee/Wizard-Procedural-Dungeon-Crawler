// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Wizard/WizardCharacter.h"
#include "WizardPlayer.generated.h"

class UPhysicsHandleComponent;

UENUM()
enum class EHandsVisibility
{
	E_Hidden,
	E_Transition,
	E_Visible
};

class UInputMappingContext;
class UBagComponent;
class UWidgetInteractionComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUILeftRightInput, int, Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandsVisibilityChanged, bool, bHandsVisible);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewInteractionTarget, FName, ItemName, FName, InteractionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToggleBagRequest, bool, bIsOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneSlotSelected, int, RuneIdx);

UCLASS(Blueprintable, BlueprintType)
class AWizardPlayer : public AWizardCharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnUILeftRightInput OnUILeftRightInput;

	UPROPERTY(BlueprintAssignable)
	FOnHandsVisibilityChanged OnHandsVisibilityChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionTarget OnNewInteractionTarget;
	UPROPERTY(BlueprintAssignable)
	FOnToggleBagRequest OnToggleBagRequest;
	UPROPERTY(BlueprintAssignable)
	FOnRuneSlotSelected OnRuneSlotSelected;
	
	AWizardPlayer();
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;

	virtual AActor* Interact(AActor* Actor) override;
	
	virtual APickupItem* GrabItem(AActor* Actor, UPrimitiveComponent* ActorComponent);
	virtual FTransform GetGrabTargetTransform();
	virtual APickupItem* ReleaseItem();
	
	UFUNCTION(BlueprintCallable)
	virtual void SetHandsVisibility(const bool bState);
	UFUNCTION(BlueprintCallable)
	bool AreHandsVisible() const;
private:
	// Input
	UFUNCTION()
	void UpdateBagInputContext(bool IsBagOpen);
	UFUNCTION()
	void UpdateSpellBookInputContext(bool IsSpellBookOpen);
	UFUNCTION()
	void UpdateMapInputContext(bool IsMapOpenOpen);
	
	void SetInteractionInput(bool bState) const;
	void SetCharacterMovementInput(bool bState) const;

	// Interaction Input
	UFUNCTION()
	void OnPrimaryHandAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnInteractAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnHoldItemAction(const FInputActionValue& Value);

	// Movement Input
	UFUNCTION()
	void OnMoveAroundAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnLookAroundAction(const FInputActionValue& Value);
	UFUNCTION()
	void OnSetSprintAction(const FInputActionValue& Value);
	
	// Bag Input
	UFUNCTION()
	void OnToggleBagAction(const FInputActionValue& Value);

	// SpellBook Input
	UFUNCTION()
	void OnRuneSlotKeyPressed(const FInputActionValue& Value);
	
public:
	// Components
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ArmsMeshComponent;
	UPROPERTY(EditAnywhere)
	UPhysicsHandleComponent* PhysicsHandleComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	USceneComponent* BagSocket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	UBagComponent* Bag;
	
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
	TSoftObjectPtr<UInputAction> HoldItem_InputAction;
	
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

protected:
	TWeakObjectPtr<APickupItem> HoldingActor = nullptr;
	
	FHitResult InteractionTarget;
	EHandsVisibility HandsVisibility = EHandsVisibility::E_Hidden;

	bool bNewHoldingItem = false;
};
