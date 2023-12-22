// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/WizardPlayer.h"
#include "Characters/Player/WizardPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/Character/BagComponent.h"
#include "Components/Character/PawnStats.h"
#include "Components/Character/PlayerInteractionRaycast.h"
#include "Components/Character/SpellbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UI/InteractionUI.h"
#include "UI/Wizard/RuneCastsHistory.h"
#include "UI/Wizard/WizardHUD.h"

AWizardPlayer::AWizardPlayer(): Super()
{
	CameraArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraArmComponent");
	CameraArmComponent->SetupAttachment(HandsRootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(CameraArmComponent);

	RightHandSocketComponent->SetupAttachment(CameraComponent);
	LeftHandSocketComponent->SetupAttachment(CameraComponent);
	
	PhysicsHandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent>("PhysicsHandle");
	
	ArmsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMeshComponent");
	ArmsMeshComponent->SetupAttachment(CameraArmComponent, NAME_None);
	ArmsMeshComponent->bCastDynamicShadow = false;
	ArmsMeshComponent->CastShadow = false;
	ArmsMeshComponent->SetOnlyOwnerSee(true);

	PlayerInteraction = CreateDefaultSubobject<UPlayerInteractionRaycast>(FName("PlayerInteraction"));
	PlayerInteraction->SetupAttachment(CameraArmComponent);
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;
}

void AWizardPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!FMath::IsNearlyEqual(CameraComponent->FieldOfView,TargetCameraFieldOfView, 0.01))
	{
		CameraComponent->FieldOfView = FMath::Lerp(
			CameraComponent->FieldOfView,
			TargetCameraFieldOfView,
			10.f * DeltaSeconds
			);
	}
	else
	{
		CameraComponent->FieldOfView = TargetCameraFieldOfView;
	}
	
	PlayerInteraction->UpdateInteractionTarget(CameraComponent->GetForwardVector());
	
	if (PlayerInteraction->IsGrabbingItem())
	{
		const FTransform GrabTransform = PlayerInteraction->GetGrabTargetTransform();
		PhysicsHandleComponent->SetTargetLocationAndRotation(
			GrabTransform.GetLocation(),
			GrabTransform.GetRotation().Rotator()
			);
	}
}

void AWizardPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// set ui inputs
	if (AWizardPlayerController* PlayerController = Cast<AWizardPlayerController>(GetController()))
	{
		PlayerController->SetupDefaultInput(PlayerInputComponent);

		PlayerController->OnGrabbedActionInput.AddDynamic(PlayerInteraction, &UPlayerInteractionRaycast::GrabInteractionTarget);
		PlayerController->OnReleasedActionInput.AddDynamic(PlayerInteraction, &UPlayerInteractionRaycast::Release);
		// Setup WizardHUD
		if (UWizardHUD* WizardHUD = PlayerController->GetWizardHud())
		{
			// Bind stat changes to hud
			WizardStats->OnHurt.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerHurt);
			WizardStats->OnHeal.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerHeal);
			WizardStats->OnManaUsage.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerManaUsage);
			WizardStats->Heal(0.f);
			WizardStats->UseMana(nullptr, 0.f);

			PlayerInteraction->OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);
			
			SpellBook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::BindRuneToSlot);
			SpellBook->OnRuneCasted.AddDynamic(WizardHUD, &UWizardHUD::OnRuneCasted);
		}
		// Bind interaction system
		PlayerInteraction->OnItemPickedUp.AddDynamic(this, &AWizardPlayer::UseItem);
		PlayerInteraction->OnRunePickedUp.AddDynamic(SpellBook, &USpellbookComponent::AddRune);
		
		PlayerController->SetInputContext(EInputContextType::Gameplay, true);
	}
}

void AWizardPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AWizardPlayer::UpdateSpellBookInputContext(bool IsSpellBookOpen)
{
	//TODO: Add Bag specific actions

	SetInteractionInput(!IsSpellBookOpen);
	SetCharacterMovementInput(!IsSpellBookOpen);
}

void AWizardPlayer::UpdateMapInputContext(bool IsMapOpenOpen)
{
	//TODO: Add Bag specific actions

	SetInteractionInput(!IsMapOpenOpen);
	SetCharacterMovementInput(!IsMapOpenOpen);
}

void AWizardPlayer::OnInteractAction(const FInputActionValue& Value)
{
	if (PlayerInteraction->IsGrabbingItem() || PlayerInteraction->bInteractionDisabled)
	{
		return;
	}
	
	bool bInteractionState = PlayerInteraction->Interact();
}

void AWizardPlayer::OnMoveAroundAction(const FInputActionValue& Value)
{
	const FVector2D MoveDirection = Value.Get<FVector2D>();
	MoveAround(MoveDirection);
}

void AWizardPlayer::OnLookAroundAction(const FInputActionValue& Value)
{
	LookAround(Value.Get<FVector2D>());
}

void AWizardPlayer::OnSetSprintAction(const FInputActionValue& Value)
{
	SetSprinting(Value.Get<bool>());
}

void AWizardPlayer::OnSetCrouchAction(const FInputActionValue& Value)
{
	SetCrouch(!IsCrouching());
}

void AWizardPlayer::OnToggleBagAction(const FInputActionValue& Value)
{
	if (OnToggleBagRequest.IsBound())
	{
		const bool IsBagOpen = Bag->IsOpen();
		SetCrouch(!IsBagOpen);
		if (IsBagOpen)
		{
			OnUILeftRightInput.AddDynamic(Bag, &UBagComponent::OnLeftRightInputAction);
		}
		else
		{
			OnUILeftRightInput.RemoveDynamic(Bag, &UBagComponent::OnLeftRightInputAction);
		}
		OnToggleBagRequest.Broadcast(!IsBagOpen);
	}
}

void AWizardPlayer::OnRuneSlotKeyPressed(const FInputActionValue& Value)
{
	const int32 RuneSlotIdx = ((int32)Value.Get<float>()) - 1; // "1 key" is equal to scalar of 1 because 0 is ignored in input.
	if (OnRuneSlotSelected.IsBound()) {
		OnRuneSlotSelected.Broadcast(RuneSlotIdx);
	}
}
