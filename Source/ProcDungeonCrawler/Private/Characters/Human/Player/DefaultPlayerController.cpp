// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Player/DefaultPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Human/Human.h"
#include "Kismet/GameplayStatics.h"


void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ADefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ADefaultPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!Movement_InputContext.IsNull() && !Movement_InputContext.IsValid())
	{
		Movement_InputContext.LoadSynchronous();
	}
	if (!Interaction_InputContext.IsNull() && !Interaction_InputContext.IsValid())
	{
		Interaction_InputContext.LoadSynchronous();
	}
	if (!UI_InputContext.IsNull() && !UI_InputContext.IsValid())
	{
		UI_InputContext.LoadSynchronous();
	}
}

void ADefaultPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ADefaultPlayerController::SetInputContext(EInputContextType InputContext, bool bState)
{
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (EnhancedInputSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputSubsystem is null"));
		return;
	}
	
	UInputMappingContext* TargetInputContext = nullptr;
	switch (InputContext)
	{
	case EInputContextType::Movement:
		if (!Movement_InputContext.IsNull() && !Movement_InputContext.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("Dupa movement"));
			TargetInputContext = Movement_InputContext.LoadSynchronous();
		}
		else
		{
			TargetInputContext = Movement_InputContext.Get();
		}
		break;
	case EInputContextType::Interaction:
		if (!Interaction_InputContext.IsNull() && !Interaction_InputContext.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("Dupa interaction"));
			TargetInputContext = Interaction_InputContext.LoadSynchronous();
		}
		else
		{
			TargetInputContext = Interaction_InputContext.Get();
		}
		break;
	case EInputContextType::UI:
		if (!UI_InputContext.IsNull() && !UI_InputContext.IsValid())
		{
			UE_LOG(LogTemp, Display, TEXT("Dupa ui"))
			TargetInputContext = UI_InputContext.LoadSynchronous();
		}
		else
		{
			TargetInputContext = UI_InputContext.Get();
		}
		break;
	}
	
	if (bState)
	{
		EnhancedInputSubsystem->AddMappingContext(TargetInputContext, 0);
	}
	else
	{
		EnhancedInputSubsystem->RemoveMappingContext(TargetInputContext);
	}
}

void ADefaultPlayerController::SetInteractionInput(bool bState) const
{
	
}

void ADefaultPlayerController::SetCharacterMovementInput(bool bState) const
{
	
}

void ADefaultPlayerController::OnPrimaryHandAction(const FInputActionValue& Value)
{
	if (OnPrimaryHandActionInput.IsBound())
	{
		OnPrimaryHandActionInput.Broadcast();
	}
}

void ADefaultPlayerController::OnInteractAction(const FInputActionValue& Value)
{
	if (OnInteractActionInput.IsBound())
	{
		OnInteractActionInput.Broadcast();
	}
}

void ADefaultPlayerController::OnGrabItemAction(const FInputActionValue& Value)
{
	if (OnGrabbedActionInput.IsBound())
	{
		OnGrabbedActionInput.Broadcast();
	}
}

void ADefaultPlayerController::OnReleaseItemAction(const FInputActionValue& Value)
{
	if (OnReleasedActionInput.IsBound())
	{
		OnReleasedActionInput.Broadcast();
	}
}

void ADefaultPlayerController::OnJumpAction(const FInputActionValue& Value)
{
	if (OnJumpActionInput.IsBound())
	{
		OnJumpActionInput.Broadcast();
	}
}

void ADefaultPlayerController::OnToggleBagAction(const FInputActionValue& Value)
{
	if (OnToggleBagRequest.IsBound())
	{
		OnToggleBagRequest.Broadcast();
	}
}

void ADefaultPlayerController::OnRuneSlotKeyPressed(const FInputActionValue& Value)
{
	int RuneSlotIdx = static_cast<int>(Value.Get<float>());
	
	if (OnRuneSlotSelected.IsBound())
	{
		OnRuneSlotSelected.Broadcast(RuneSlotIdx);
	}
}

void ADefaultPlayerController::SetupDefaultInput(UEnhancedInputComponent* PlayerInputComponent)
{
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (EnhancedInputSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputSubsystem is null"));
		return;
	}
	
	// Set movement inputs
	PlayerInputComponent->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnLookAroundInputAction);
	PlayerInputComponent->BindAction(MoveAround_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnMoveAroundInputAction);
	PlayerInputComponent->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSetSprintInputAction);
	PlayerInputComponent->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnJumpInputAction);
	PlayerInputComponent->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSetCrouchInputAction);

	// set interaction inputs
	PlayerInputComponent->BindAction(UseWeapon_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnUseWeaponInputAction);
	PlayerInputComponent->BindAction(CastPreparedSpell_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnCastPreparedSpellInputAction);
	PlayerInputComponent->BindAction(UseItemInHand_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnUseItemInHandInputAction);
	PlayerInputComponent->BindAction(CastRuneFromSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnCastRuneFromSlotInputAction);
	PlayerInputComponent->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnInteractionInputAction);
	PlayerInputComponent->BindAction(GrabItem_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnGrabItemInputAction);
	
	// set ui inputs
	PlayerInputComponent->BindAction(ToggleInventory_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleInventoryInputAction);
	PlayerInputComponent->BindAction(ToggleSpellBook_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleSpellBookInputAction);
	PlayerInputComponent->BindAction(TogglePauseMenu_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnTogglePauseMenuInputAction);
	PlayerInputComponent->BindAction(ToggleMap_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleMapInputAction);
}


void ADefaultPlayerController::OnLookAroundInputAction(const FInputActionValue& Value)
{
	const FVector2D LookOffset = Value.Get<FVector2D>();

	if (OnLookAroundInput.IsBound())
	{
		OnLookAroundInput.Broadcast(LookOffset);
	}
}

void ADefaultPlayerController::OnMoveAroundInputAction(const FInputActionValue& Value)
{
	const FVector2D MoveOffset = Value.Get<FVector2D>();

	if (OnMoveAroundInput.IsBound())
	{
		OnMoveAroundInput.Broadcast(MoveOffset);
	}
}



void ADefaultPlayerController::OnSetSprintInputAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	if (OnSprintToggled.IsBound())
	{
		OnSprintToggled.Broadcast(bIsPressed);
	}
}

void ADefaultPlayerController::OnJumpInputAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	bool bNewIsCrouching = false;

	if (const AHuman* WizardCharacter = Cast<AHuman>(GetPawn()))
	{
		bNewIsCrouching = !WizardCharacter->bIsCrouched;
	}
	
	if (OnCrouchToggled.IsBound())
	{
		OnCrouchToggled.Broadcast(bNewIsCrouching);
	}
}

void ADefaultPlayerController::OnSetCrouchInputAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	bool bNewIsCrouching = false;

	if (const AHuman* WizardCharacter = Cast<AHuman>(GetPawn()))
	{
		bNewIsCrouching = !WizardCharacter->bIsCrouched;
	}
	
	if (OnCrouchToggled.IsBound())
	{
		OnCrouchToggled.Broadcast(bNewIsCrouching);
	}
}

UPlayerHUD* ADefaultPlayerController::SetupWizardHud()
{
	if (WizardHUD)
	{
		WizardHUD->RemoveFromParent();
	}
	
	if (WizardHUDClass)
	{
		WizardHUD = Cast<UPlayerHUD>(CreateWidget<UUserWidget>(this, WizardHUDClass));
		if (WizardHUD)
		{
			WizardHUD->AddToViewport();
		}
	}

	return WizardHUD;
}
