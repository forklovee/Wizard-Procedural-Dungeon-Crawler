// Fill out your copyright notice in the Description page of Project Settings.

#include "WizardCharacter.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Player/WizardPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/Character/BagComponent.h"
#include "Components/Character/SpellbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Spell/RunePickup.h"
#include "Spell/SpellCast.h"

#include "UI/Wizard/WizardHUD.h"
#include "UI/InteractionUI.h"

#include "Interface/PropPickupInterface.h"

AWizardCharacter::AWizardCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;
	
	ArmsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMeshComponent");
	ArmsMeshComponent->SetupAttachment(CameraComponent, NAME_None);
	ArmsMeshComponent->bCastDynamicShadow = false;
	ArmsMeshComponent->CastShadow = false;
	ArmsMeshComponent->SetOnlyOwnerSee(true);

	// Hand sockets
	LeftHandSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("LeftHandSocket"));
	LeftHandSocketComponent->SetupAttachment(CameraComponent, FName("LeftHand"));
	RightHandSocketComponent = CreateDefaultSubobject<USceneComponent>(FName("RightHandSocket"));
	RightHandSocketComponent->SetupAttachment(CameraComponent, FName("RightHand"));

	// Widget interaction
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(FName("WidgetInteractionComponent"));
	WidgetInteractionComponent->SetupAttachment(CameraComponent);

	// Bag with items and runes
	Bag = CreateDefaultSubobject<UBagComponent>(FName("Bag"));
	Bag->SetupAttachment(RootComponent);
	Spellbook = CreateDefaultSubobject<USpellbookComponent>(FName("Spellbook"));
	Spellbook->SetupAttachment(RightHandSocketComponent);
}

void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementSpeed = WalkingSpeed;
	
	if (AWizardPlayerController* PlayerController = Cast<AWizardPlayerController>(GetController()))
	{
		UWizardHUD* WizardHUD = PlayerController->SetupWizardHud();
		
		Spellbook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::AddRuneToUI);
		OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);
		OnNewInteractionTarget.Broadcast(NAME_None, NAME_None);
	}
}

// INPUT

void AWizardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Set action inputs
	Input->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::LookAround);
	Input->BindAction(Movement_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::MoveAround);
	Input->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ToggleSprint);
	Input->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Jump);
	// Input->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Crouch);

	// set magic inputs
	Input->BindAction(RuneCast_InputAction.Get(), ETriggerEvent::Triggered, Spellbook, &USpellbookComponent::CastRune);
	// Input->BindAction(ChangeSpellSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ChangeSpellSlot);
	
	// set interaction inputs
	Input->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::PrimaryAction);
	Input->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Interact);

	// set bag inputs
	Input->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, Bag, &UBagComponent::ToggleBag);
	Input->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
	
	// set ui inputs
	if (const AWizardPlayerController* PlayerController = Cast<AWizardPlayerController>(GetController()))
	if (UWizardHUD* WizardHUD = PlayerController->GetWizardHud())
	{
		Input->BindAction(OpenMap_InputAction.Get(), ETriggerEvent::Triggered, WizardHUD, &UWizardHUD::OpenMap);
		Input->BindAction(OpenMap_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
		
		Input->BindAction(OpenSpellBook_InputAction.Get(), ETriggerEvent::Triggered, WizardHUD, &UWizardHUD::OpenSpellbook);
		Input->BindAction(OpenSpellBook_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
	}

	// set default input context
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	if (!MouseMoveAndUI_Context.IsNull())
	{
		const UInputMappingContext* InteractionContext = MouseMoveAndUI_Context.LoadSynchronous();
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnhancedInputSubsystem->AddMappingContext(InteractionContext, 0);
		}
	}

	// set the rest of input contexts
	SetInteractionInput(true);
	SetCharacterMovementInput(true);
}

void AWizardCharacter::SetInteractionInput(bool bState) const
{
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	if (!Interaction_InputContext.IsNull())
	{
		const UInputMappingContext* InteractionContext = Interaction_InputContext.LoadSynchronous();
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (bState)
			{
				EnhancedInputSubsystem->AddMappingContext(InteractionContext, 2);
			}
			else
			{
				EnhancedInputSubsystem->RemoveMappingContext(InteractionContext);
			}
		}
	}
}

void AWizardCharacter::SetCharacterMovementInput(bool bState) const
{
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	if (!CharacterMovement_InputContext.IsNull())
	{
		const UInputMappingContext* CharacterMovementContext = CharacterMovement_InputContext.LoadSynchronous();
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (bState)
			{
				EnhancedInputSubsystem->AddMappingContext(CharacterMovementContext, 1);
			}
			else
			{
				EnhancedInputSubsystem->RemoveMappingContext(CharacterMovementContext);
			}
		}
	}
}

void AWizardCharacter::UpdateInputContexts(const FInputActionValue& Value)
{
	const bool bContextsState = Bag->IsOpen();
	SetInteractionInput(!bContextsState);
	SetCharacterMovementInput(!bContextsState);
}

// INPUT

void AWizardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FHitResult CurrentHitResult = InteractionTarget;
	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		CameraComponent->GetComponentLocation(),
		CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * 300.f,
		20,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{},
		EDrawDebugTrace::None,
		InteractionTarget,
		true
	);

	if (OnNewInteractionTarget.IsBound() && CurrentHitResult.GetActor() != InteractionTarget.GetActor())
	{
		if (InteractionTarget.GetActor() != nullptr && InteractionTarget.GetActor()->Implements<UPropPickupInterface>())
		{
			OnNewInteractionTarget.Broadcast(
				IPropPickupInterface::Execute_GetItemName(InteractionTarget.GetActor()),
				IPropPickupInterface::Execute_GetInteractionName(InteractionTarget.GetActor())
				);
		}
		else
		{
			OnNewInteractionTarget.Broadcast(NAME_None, NAME_None);
		}
	}
}

void AWizardCharacter::MoveAround(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Axis2D)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}
	
	const FVector2D MovementInput = Value.Get<FVector2D>();
	if (Controller == nullptr) return;
	
	AddMovementInput(GetActorRightVector(), MovementInput.Y);
	AddMovementInput(GetActorForwardVector(), MovementInput.X);
}

void AWizardCharacter::LookAround(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Axis2D)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}

	const FVector2D MouseMovementVector = Value.Get<FVector2D>();
	
	if (Controller == nullptr) return;

	AddControllerYawInput(MouseMovementVector.X);
	AddControllerPitchInput(-MouseMovementVector.Y);
}

void AWizardCharacter::ToggleSprint(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Boolean)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}
	
	bIsSprinting = Value.Get<bool>();
	
	MovementSpeed = bIsSprinting ? RunningSpeed : WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AWizardCharacter::PrimaryAction(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Boolean)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}
	
	// No prepared spell.
	if (!Spellbook->IsSpellPrepared()) return;
	Spellbook->CastPreparedSpell(this);
}

void AWizardCharacter::Interact(const FInputActionValue& Value)
{
	AActor* TargetActor = InteractionTarget.GetActor();
	if (TargetActor == nullptr) return;

	if (!TargetActor->Implements<UPropPickupInterface>())
	{
		return;
	}

	AActor* PickedUpActor = IPropPickupInterface::Execute_Pickup(TargetActor, this);

	// Add rune if rune pickup
	if (const ARunePickup* RunePickup = Cast<ARunePickup>(PickedUpActor))
	{
		Spellbook->AddRune(RunePickup->RuneCast);
	}

	// Remove actor
	PickedUpActor->Destroy();
}

