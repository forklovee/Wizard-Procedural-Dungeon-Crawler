// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/WizardPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Characters/Player/WizardPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Wizard/WizardHUD.h"


void AWizardPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AWizardPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AWizardPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AWizardPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AWizardPlayerController::OnPrimaryHandAction(const FInputActionValue& Value)
{
	
}

void AWizardPlayerController::OnInteractAction(const FInputActionValue& Value)
{
	
}

void AWizardPlayerController::OnGrabItemAction(const FInputActionValue& Value)
{
	
}

void AWizardPlayerController::OnReleaseItemAction(const FInputActionValue& Value)
{
	
}

void AWizardPlayerController::OnJumpAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();

	if (OnJumpActionInput.IsBound())
	{
		OnJumpActionInput.Broadcast();
	}
}

void AWizardPlayerController::SetupDefaultInput(UEnhancedInputComponent* PlayerInputComponent)
{
	// Set action inputs
	PlayerInputComponent->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnLookAroundAction);
	PlayerInputComponent->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnSetSprintAction);
	PlayerInputComponent->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnJumpAction);
	PlayerInputComponent->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnSetCrouchAction);

	// set magic inputs
	PlayerInputComponent->BindAction(RuneCast_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnRuneSlotKeyPressed);
	// Input->BindAction(ChangeSpellSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ChangeSpellSlot);
	
	// set interaction inputs
	PlayerInputComponent->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnPrimaryHandAction);
	PlayerInputComponent->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnInteractAction);
	PlayerInputComponent->BindAction(GrabItemBegin_InputAction.Get(), ETriggerEvent::Completed, this, &AWizardPlayerController::OnGrabItemAction);
	// Input->BindAction(GrabItemEnd_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnGrabItemAction);

	// set bag inputs
	PlayerInputComponent->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayerController::OnToggleBagAction);
	// Input->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
		
}

void AWizardPlayerController::SetInputContext(EInputContextType InputContext, bool bState)
{
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (EnhancedInputSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputSubsystem is null"));
		return;
	}
	
	const uint8 InputContextIdx = static_cast<uint8>(InputContext);
	for (uint8 BitIdx = 0; BitIdx < 2; ++BitIdx) // 2 is the number of input contexts
	{
		TWeakObjectPtr<UInputMappingContext> TargetInputContext = nullptr;
		switch (BitIdx)
		{
		case 0: // Gameplay
			if (!Gameplay_InputContext.IsNull() && !Gameplay_InputContext.IsValid())
			{
				TargetInputContext = Gameplay_InputContext.LoadSynchronous();
			}
			TargetInputContext = Gameplay_InputContext.Get();
			break;
		case 1: // UI
			if (!UI_InputContext.IsNull() && !UI_InputContext.IsValid())
			{
				TargetInputContext = UI_InputContext.LoadSynchronous();
			}
			TargetInputContext = UI_InputContext.Get();
			break;
		}
		
		if (!TargetInputContext.IsValid()) continue;

		const uint8 InputContextIdx = static_cast<uint8>(InputContext);
		const bool bIsBitSet = (InputContextIdx & (1 << BitIdx)) != 0;

		if (bIsBitSet == bState) continue;
		
		if (!bIsBitSet)
		{
			EnhancedInputSubsystem->AddMappingContext(TargetInputContext.Get(), 0);
		}
		else
		{
			EnhancedInputSubsystem->RemoveMappingContext(TargetInputContext.Get());
		}
	}
}

void AWizardPlayerController::OnMoveAroundAction(const FInputActionValue& Value)
{
	const FVector2D MoveOffset = Value.Get<FVector2D>();

	if (OnMoveAroundInput.IsBound())
	{
		OnMoveAroundInput.Broadcast(MoveOffset);
	}
}

void AWizardPlayerController::OnLookAroundAction(const FInputActionValue& Value)
{
	const FVector2D LookOffset = Value.Get<FVector2D>();

	if (OnLookAroundInput.IsBound())
	{
		OnLookAroundInput.Broadcast(LookOffset);
	}
}

void AWizardPlayerController::OnSetSprintAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	if (OnSprintToggled.IsBound())
	{
		OnSprintToggled.Broadcast(bIsPressed);
	}
}

void AWizardPlayerController::OnSetCrouchAction(const FInputActionValue& Value)
{
	const bool bIsPressed = Value.Get<bool>();
	if (OnCrouchToggled.IsBound())
	{
		OnCrouchToggled.Broadcast(bIsPressed);
	}
}

UWizardHUD* AWizardPlayerController::SetupWizardHud()
{
	if (WizardHUD)
	{
		WizardHUD->RemoveFromParent();
	}
	
	if (WizardHUDClass)
	{
		WizardHUD = Cast<UWizardHUD>(CreateWidget<UUserWidget>(this, WizardHUDClass));
		if (WizardHUD)
		{
			WizardHUD->AddToViewport();
		}
	}

	return WizardHUD;
}
