// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Player/DefaultPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Human/Human.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Wizard/PlayerHUD.h"


ADefaultPlayerController::ADefaultPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();

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

void ADefaultPlayerController::SetInputContext(EInputContextType InputContext, bool bState)
{
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	UInputMappingContext* TargetInputContext = nullptr;
	switch (InputContext)
	{
	case EInputContextType::Movement:
		if (!Movement_InputContext.IsNull() && !Movement_InputContext.IsValid())
		{
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

void ADefaultPlayerController::SetupDefaultInput(UEnhancedInputComponent* PlayerInputComponent)
{
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	EnhancedInputSubsystem->ClearAllMappings();
	
	// Set movement inputs
	PlayerInputComponent->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnLookAroundInputAction);
	PlayerInputComponent->BindAction(MoveAround_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnMoveAroundInputAction);
	PlayerInputComponent->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSetSprintInputAction);
	PlayerInputComponent->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnJumpInputAction);
	PlayerInputComponent->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSetCrouchInputAction);

	// set interaction inputs
	PlayerInputComponent->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnPrimaryInputAction);
	PlayerInputComponent->BindAction(SecondaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSecondaryInputAction);
	PlayerInputComponent->BindAction(OnRuneSlotSelected_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnRuneSlotSelectedInputAction);
	PlayerInputComponent->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnInteractionInputAction);
	PlayerInputComponent->BindAction(GrabItem_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnGrabItemInputAction);
	
	// set ui inputs
	PlayerInputComponent->BindAction(ToggleInventory_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleInventoryInputAction);
	PlayerInputComponent->BindAction(ToggleSpellBook_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleSpellBookInputAction);
	PlayerInputComponent->BindAction(TogglePauseMenu_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnTogglePauseMenuInputAction);
	PlayerInputComponent->BindAction(ToggleMap_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleMapInputAction);
}

UPlayerHUD* ADefaultPlayerController::AddHudToViewport()
{
	if (WizardHUD)
	{
		WizardHUD->RemoveFromParent();
	}
	if (PlayerHUDClass)
	{
		WizardHUD = Cast<UPlayerHUD>(CreateWidget<UUserWidget>(this, PlayerHUDClass));
		if (WizardHUD)
		{
			WizardHUD->AddToViewport();
		}
	}
	return WizardHUD;
}

// input actions handling
void ADefaultPlayerController::OnLookAroundInputAction(const FInputActionValue& Value)
{
	const FVector2D LookOffset = Value.Get<FVector2D>();

	if (OnLookAroundAction.IsBound())
	{
		OnLookAroundAction.Broadcast(LookOffset);
	}
}

void ADefaultPlayerController::OnMoveAroundInputAction(const FInputActionValue& Value)
{
	const FVector2D MoveOffset = Value.Get<FVector2D>();

	if (OnMoveAroundAction.IsBound())
	{
		OnMoveAroundAction.Broadcast(MoveOffset);
	}
}



void ADefaultPlayerController::OnSetSprintInputAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	if (OnSprintToggledAction.IsBound())
	{
		OnSprintToggledAction.Broadcast(bIsPressed);
	}
}

void ADefaultPlayerController::OnJumpInputAction(const FInputActionValue& Value)
{
	if (OnJumpAction.IsBound())
	{
		OnJumpAction.Broadcast();
	}
}

void ADefaultPlayerController::OnSetCrouchInputAction(const FInputActionValue& Value)
{
	if (OnCrouchToggledAction.IsBound())
	{
		OnCrouchToggledAction.Broadcast();
	}
}

void ADefaultPlayerController::OnPrimaryInputAction(const FInputActionValue& Value)
{
	if (OnPrimaryAction.IsBound())
	{
		OnPrimaryAction.Broadcast();
	}
}

void ADefaultPlayerController::OnSecondaryInputAction(const FInputActionValue& Value)
{
	if (OnSecondaryAction.IsBound())
	{
		OnAltAction.Broadcast();
	}
}

void ADefaultPlayerController::OnRuneSlotSelectedInputAction(const FInputActionValue& Value)
{
	const int SlotIndex = static_cast<int>(Value.Get<float>());
	
	if (OnRuneSlotSelectedAction.IsBound())
	{
		OnCrouchToggledAction.Broadcast();
	}
}

void ADefaultPlayerController::OnInteractionInputAction(const FInputActionValue& Value)
{
	if (OnInteractAction.IsBound())
	{
		OnInteractAction.Broadcast();
	}
}

void ADefaultPlayerController::OnGrabItemInputAction(const FInputActionValue& Value)
{
	if (OnGrabbedAction.IsBound())
	{
		OnGrabbedAction.Broadcast();
	}
}

void ADefaultPlayerController::OnReleasedItemInputAction(const FInputActionValue& Value)
{
	if (OnReleasedAction.IsBound())
	{
		OnReleasedAction.Broadcast();
	}
}

void ADefaultPlayerController::OnToggleInventoryInputAction(const FInputActionValue& Value)
{
	if (OnToggleInventoryAction.IsBound())
	{
		OnToggleInventoryAction.Broadcast();
	}
}

void ADefaultPlayerController::OnToggleSpellBookInputAction(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	
	if (OnToggleSpellBookAction.IsBound())
	{
		OnToggleSpellBookAction.Broadcast(bIsPressed);
	}
}

void ADefaultPlayerController::OnTogglePauseMenuInputAction(const FInputActionValue& Value)
{
	if (OnTogglePauseMenuAction.IsBound())
	{
		OnTogglePauseMenuAction.Broadcast();
	}
}

void ADefaultPlayerController::OnToggleMapInputAction(const FInputActionValue& Value)
{
	if (OnToggleMapAction.IsBound())
	{
		OnToggleMapAction.Broadcast();
	}
}
