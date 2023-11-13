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
	
	// Widget interaction
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(FName("WidgetInteractionComponent"));
	WidgetInteractionComponent->SetupAttachment(CameraArmComponent);

	// Bag with items and runes
	BagSocket = CreateDefaultSubobject<USceneComponent>(FName("BagSocket"));
	BagSocket->SetupAttachment(RootComponent);
	
	Bag = CreateDefaultSubobject<UBagComponent>(FName("Bag"));
	Bag->SetupAttachment(BagSocket);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;
}

void AWizardPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CameraComponent->FieldOfView != TargetCameraFieldOfView)
	{
		CameraComponent->FieldOfView = FMath::Lerp(
			CameraComponent->FieldOfView,
			TargetCameraFieldOfView,
			10.f * DeltaSeconds
			);
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
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Set action inputs
	Input->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnLookAroundAction);
	Input->BindAction(Movement_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnMoveAroundAction);
	Input->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnSetSprintAction);
	Input->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Jump);
	Input->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnSetCrouchAction);

	// set magic inputs
	Input->BindAction(RuneCast_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnRuneSlotKeyPressed);
	// Input->BindAction(ChangeSpellSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ChangeSpellSlot);
	
	// set interaction inputs
	Input->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnPrimaryHandAction);
	Input->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnInteractAction);
	Input->BindAction(GrabItemBegin_InputAction.Get(), ETriggerEvent::Completed, this, &AWizardPlayer::OnGrabItemAction);
	// Input->BindAction(GrabItemEnd_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnGrabItemAction);

	// set bag inputs
	Input->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnToggleBagAction);
	// Input->BindAction(OpenBag_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
	
	// set ui inputs
	if (const AWizardPlayerController* PlayerController = Cast<AWizardPlayerController>(GetController()))
	if (UWizardHUD* WizardHUD = PlayerController->GetWizardHud())
	{
		// Input->BindAction(OpenMap_InputAction.Get(), ETriggerEvent::Triggered, WizardHUD, &UWizardHUD::OpenMap);
		// Input->BindAction(OpenMap_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
		
		// Input->BindAction(OpenSpellBook_InputAction.Get(), ETriggerEvent::Triggered, WizardHUD, &UWizardHUD::OpenSpellbook);
		// Input->BindAction(OpenSpellBook_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::UpdateInputContexts);
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

void AWizardPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (AWizardPlayerController* PlayerController = Cast<AWizardPlayerController>(GetController()))
	{
		UWizardHUD* WizardHUD = PlayerController->SetupWizardHud();

		WizardStats->OnHurt.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerHurt);
		WizardStats->OnHeal.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerHeal);
		WizardStats->OnManaUsage.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerManaUsage);
		WizardStats->UpdateUIStats();
		
		PlayerInteraction->OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);
		PlayerInteraction->OnItemPickedUp.AddDynamic(Bag, &UBagComponent::AddItem);
		PlayerInteraction->OnRunePickedUp.AddDynamic(SpellBook, &USpellbookComponent::AddRune);
		
		Bag->OnBagStateChanged.AddDynamic(this, &AWizardPlayer::UpdateBagInputContext);
		
		OnRuneSlotSelected.AddDynamic(WizardHUD, &UWizardHUD::UseRuneOfIdx);
		OnRuneSlotSelected.AddDynamic(SpellBook, &USpellbookComponent::CastRuneOfIdx);
		SpellBook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::BindRuneToSlot);
		SpellBook->OnCastedRunesCleared.AddDynamic(WizardHUD->CastedRuneHistory, &URuneCastsHistory::ClearCastHistory);

		WidgetInteractionComponent->OnHoveredWidgetChanged.AddDynamic(Bag, &UBagComponent::OnPlayerCursorOnWidgetHoverChanged);
	}
}

void AWizardPlayer::OnGrabItemAction(const FInputActionValue& Value)
{
	if (PlayerInteraction->IsGrabbingItem() || !PlayerInteraction->CanGrabTarget())
	{
		return;
	}
	
	if (UPrimitiveComponent* ActorComponent = PlayerInteraction->Grab())
	{
		const FTransform GrabTransform = PlayerInteraction->GetGrabTargetTransform();
		PhysicsHandleComponent->GrabComponentAtLocationWithRotation(
			ActorComponent,
			FName(""),
			GrabTransform.GetLocation(),
			GrabTransform.GetRotation().Rotator()
			);
		SetHandsVisibility(false);
	}
	else
	{
		SetHandsVisibility(true);
	}
}

void AWizardPlayer::OnReleaseItemAction(const FInputActionValue& Value)
{
	if (!PlayerInteraction->IsGrabbingItem())
	{
		return;
	}

	if (PlayerInteraction->Release())
	{
		PhysicsHandleComponent->ReleaseComponent();
	}
}

void AWizardPlayer::SetHandsVisibility(const bool bState)
{
	if (HandsVisibility == EHandsVisibility::E_Transition ||
		HandsVisibility == (bState ? EHandsVisibility::E_Visible : EHandsVisibility::E_Hidden))
	{
		return;
	}
	
	RightHandSocketComponent->SetVisibility(bState);
	LeftHandSocketComponent->SetVisibility(bState);
	HandsVisibility = bState ? EHandsVisibility::E_Visible : EHandsVisibility::E_Hidden;
}

bool AWizardPlayer::AreHandsVisible() const
{
	return HandsVisibility == EHandsVisibility::E_Visible;
}

void AWizardPlayer::UpdateBagInputContext(bool IsBagOpen)
{
	//TODO: Add Bag specific actions
	TargetCameraFieldOfView = !IsBagOpen ? 90.0f : 100.0f;
	
	PlayerInteraction->bInteractionDisabled = IsBagOpen;
	
	// SetInteractionInput(!IsBagOpen);
	SetCharacterMovementInput(!IsBagOpen);
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

void AWizardPlayer::SetInteractionInput(bool bState) const
{
	//TODO: Do this as a library function
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

void AWizardPlayer::SetCharacterMovementInput(bool bState) const
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

void AWizardPlayer::OnPrimaryHandAction(const FInputActionValue& Value)
{
	if (PlayerInteraction->IsGrabbingItem())
	{
		OnReleaseItemAction(Value);
		return;
	}
	PrimaryHandAction();
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

	if (OnUILeftRightInput.IsBound())
	{
		OnUILeftRightInput.Broadcast((int)MoveDirection.X);
	}
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
