// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Player/DefaultPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Human/Human.h"
#include "Kismet/GameplayStatics.h"
#include "..\..\..\..\Public\UI\Wizard\PlayerHUD.h"


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
	case EInputContextType::Gameplay:
		if (!Gameplay_InputContext.IsNull() && !Gameplay_InputContext.IsValid())
		{
			TargetInputContext = Gameplay_InputContext.LoadSynchronous();
		}
		else
		{
			TargetInputContext = Gameplay_InputContext.Get();
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
	
	// Set action inputs
	PlayerInputComponent->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnLookAroundAction);
	PlayerInputComponent->BindAction(Movement_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnMoveAroundAction);
	PlayerInputComponent->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSetSprintAction);
	PlayerInputComponent->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnJumpAction);
	PlayerInputComponent->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnSetCrouchAction);

	// set magic inputs
	PlayerInputComponent->BindAction(RuneCast_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnRuneSlotKeyPressed);
	// Input->BindAction(ChangeSpellSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ChangeSpellSlot);
	
	// set interaction inputs
	PlayerInputComponent->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnPrimaryHandAction);
	PlayerInputComponent->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnInteractAction);
	PlayerInputComponent->BindAction(GrabItemBegin_InputAction.Get(), ETriggerEvent::Completed, this, &ADefaultPlayerController::OnGrabItemAction);
	// Input->BindAction(GrabItemEnd_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnGrabItemAction);

	// set bag inputs
	PlayerInputComponent->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &ADefaultPlayerController::OnToggleBagAction);
	// Input->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
}



void ADefaultPlayerController::OnMoveAroundAction(const FInputActionValue& Value)
{
	const FVector2D MoveOffset = Value.Get<FVector2D>();

	if (OnMoveAroundInput.IsBound())
	{
		OnMoveAroundInput.Broadcast(MoveOffset);
	}
}

void ADefaultPlayerController::OnLookAroundAction(const FInputActionValue& Value)
{
	const FVector2D LookOffset = Value.Get<FVector2D>();

	if (OnLookAroundInput.IsBound())
	{
		OnLookAroundInput.Broadcast(LookOffset);
	}
}

void ADefaultPlayerController::OnSetSprintAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	if (OnSprintToggled.IsBound())
	{
		OnSprintToggled.Broadcast(bIsPressed);
	}
}

void ADefaultPlayerController::OnSetCrouchAction(const FInputActionValue& Value)
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
