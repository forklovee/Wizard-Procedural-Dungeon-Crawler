// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "WizardCharacter.generated.h"

class ABagActor;
class UWidgetInteractionComponent;
class UBagComponent;
class UInputMappingContext;
class UInputAction;

class ASpellCast;
class URuneCast;

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
	bool IsRuneSequenceValid(TArray<TSoftObjectPtr<URuneCast>> SpellRunes, TSubclassOf<ASpellCast>& OutSpellCastClass);
	
	void PrepareSpell(TSubclassOf<ASpellCast> SpellCastClass);

	void ClearCastedRunes();
	
	// Interaction
	void PrimaryAction(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);

	void ToggleBag(const FInputActionValue& Value);
	
	void SetSpellDataFromDataTable();
public:
	// Bag class
	UPROPERTY(EditAnywhere, Category= "Bag")
	TSubclassOf<ABagActor> BagActorClass;
	
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
	
	UPROPERTY(EditAnywhere)
	UBagComponent* Bag;

	// Input
	UPROPERTY(EditAnywhere, Category= "Spells")
	TSoftObjectPtr<class UDataTable> SpellBookDataTable;
	
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

	// 3D UI Actors
	TWeakObjectPtr<ABagActor> BagActor;
	
	// Spell oriented variables
	TWeakObjectPtr<ASpellCast> PreparedSpell;
	TArray<FHitResult> SpellTargets;
	
	// Obtained Runes
	TArray<TSoftObjectPtr<URuneCast>> Runes;

	// Runes casted
	TArray<TSoftObjectPtr<URuneCast>> CastedRunes;
	FTimerHandle ClearCastedRunesTimerHandle;

	// Spell data RuneChain id == SpellCastClass id
	// Correct SpellCast for RuneChain is get from IsRuneSequenceValid function
	TArray<TArray<TSoftObjectPtr<URuneCast>>> RuneChains;
	TArray<TSubclassOf<ASpellCast>> SpellCastClasses;
	
	bool bIsCastingSpell = false;
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkingSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category= "Movement", meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 14.0f;

	float MovementSpeed = 0.0f;
	
};
