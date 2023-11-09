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
#include "Components/Character/SpellbookComponent.h"
#include "Interface/PropPickupInterface.h"
#include "Items/PickupItem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UI/InteractionUI.h"
#include "UI/Wizard/RuneCastsHistory.h"
#include "UI/Wizard/WizardHUD.h"

AWizardPlayer::AWizardPlayer(): Super()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(HeadSocketComponent);
	CameraComponent->bUsePawnControlRotation = true;

	PhysicsHandleComponent = CreateDefaultSubobject<UPhysicsHandleComponent>("PhysicsHandle");
	
	ArmsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMeshComponent");
	ArmsMeshComponent->SetupAttachment(HeadSocketComponent, NAME_None);
	ArmsMeshComponent->bCastDynamicShadow = false;
	ArmsMeshComponent->CastShadow = false;
	ArmsMeshComponent->SetOnlyOwnerSee(true);
	
	// Widget interaction
	WidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>(FName("WidgetInteractionComponent"));
	WidgetInteractionComponent->SetupAttachment(CameraComponent);

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
	
	const FHitResult CurrentHitResult = InteractionTarget;
	if (HoldingActor.IsValid())
	{
		FVector GrabLocation = CameraComponent->GetComponentLocation() +
								CameraComponent->GetForwardVector() * 50.f;
		FRotator LookAtRotator = FRotationMatrix::MakeFromX(CameraComponent->GetForwardVector()).Rotator(); 
		PhysicsHandleComponent->SetTargetLocationAndRotation(GrabLocation, LookAtRotator);
		
		InteractionTarget = FHitResult();
		if (OnNewInteractionTarget.IsBound())
		{
			OnNewInteractionTarget.Broadcast(NAME_None, NAME_None);
		}
		return;
	}
	
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

void AWizardPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Set action inputs
	Input->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnLookAroundAction);
	Input->BindAction(Movement_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnMoveAroundAction);
	Input->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnSetSprintAction);
	Input->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Jump);
	// Input->BindAction(Crouch_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Crouch);

	// set magic inputs
	Input->BindAction(RuneCast_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnRuneSlotKeyPressed);
	// Input->BindAction(ChangeSpellSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ChangeSpellSlot);
	
	// set interaction inputs
	Input->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnPrimaryHandAction);
	Input->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnInteractAction);
	Input->BindAction(HoldItem_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardPlayer::OnHoldItemAction);

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

		OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);

		Bag->OnBagStateChanged.AddDynamic(this, &AWizardPlayer::UpdateBagInputContext);
		
		OnRuneSlotSelected.AddDynamic(WizardHUD, &UWizardHUD::UseRuneOfIdx);
		OnRuneSlotSelected.AddDynamic(SpellBook, &USpellbookComponent::CastRuneOfIdx);
		SpellBook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::BindRuneToSlot);
		SpellBook->OnCastedRunesCleared.AddDynamic(WizardHUD->CastedRuneHistory, &URuneCastsHistory::ClearCastHistory);

		OnNewInteractionTarget.Broadcast(NAME_None, NAME_None);
	}
}

AActor* AWizardPlayer::Interact(AActor* Actor)
{
	AActor* ValidInteractionActor = Super::Interact(Actor); 
	if (ValidInteractionActor == nullptr)
	{
		return nullptr;
	}

	// Add bag
	if (const APickupItem* PickupItem = Cast<APickupItem>(Actor))
	{
		Bag->AddItem(PickupItem->GetClass(), 1);
		IPropPickupInterface::Execute_Pickup(Actor, this);
	}
	
	return ValidInteractionActor;
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
	
	SetInteractionInput(!IsBagOpen);
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
	if (HoldingActor.IsValid())
	{
		return;
	}
	PrimaryHandAction();
}

void AWizardPlayer::OnInteractAction(const FInputActionValue& Value)
{
	if (bNewHoldingItem) //Cancel Interaction if Player wants to hold an object
	{
		bNewHoldingItem = false;
		return;
	}
	if (HoldingActor.IsValid()) // Drop the object
	{
		if (UStaticMeshComponent* GrabbedComponent = Cast<UStaticMeshComponent>(PhysicsHandleComponent->GetGrabbedComponent()))
		{
			GrabbedComponent->SetMassOverrideInKg(NAME_None, 1, false);
		}
		PhysicsHandleComponent->ReleaseComponent();
		
		HoldingActor = nullptr;
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Interaction!"))
	
	AActor* TargetActor = InteractionTarget.GetActor();
	Interact(TargetActor);
}

void AWizardPlayer::OnHoldItemAction(const FInputActionValue& Value)
{
	if (HoldingActor.IsValid())
	{
		return;
	}
	
	AActor* TargetActor = InteractionTarget.GetActor();
	HoldingActor = HoldItem(TargetActor);
	
	const bool bIsValid = HoldingActor.IsValid();
	if (bIsValid)
	{
		if (UStaticMeshComponent* ActorMesh = Cast<UStaticMeshComponent>(InteractionTarget.GetComponent()))
		{
			FVector GrabLocation = CameraComponent->GetComponentLocation() +
											 CameraComponent->GetForwardVector() * 100.f;
			FRotator LookAtRotator = FRotationMatrix::MakeFromX(CameraComponent->GetForwardVector()).Rotator(); 
			PhysicsHandleComponent->GrabComponentAtLocationWithRotation(ActorMesh,
				FName(""),
				GrabLocation,
				LookAtRotator);
			ActorMesh->SetMassOverrideInKg(NAME_None, 1, true);
		}
	}
	
	SetHandsVisibility(!bIsValid);
	bNewHoldingItem = bIsValid;
}

void AWizardPlayer::OnMoveAroundAction(const FInputActionValue& Value)
{
	MoveAround(Value.Get<FVector2D>());
}

void AWizardPlayer::OnLookAroundAction(const FInputActionValue& Value)
{
	LookAround(Value.Get<FVector2D>());
}

void AWizardPlayer::OnSetSprintAction(const FInputActionValue& Value)
{
	SetSprinting(Value.Get<bool>());
}

void AWizardPlayer::OnToggleBagAction(const FInputActionValue& Value)
{
	if (OnToggleBagRequest.IsBound())
	{
		OnToggleBagRequest.Broadcast(!Bag->IsOpen());
	}
}

void AWizardPlayer::OnRuneSlotKeyPressed(const FInputActionValue& Value)
{
	const int32 RuneSlotIdx = ((int32)Value.Get<float>()) - 1; // "1 key" is equal to scalar of 1 because 0 is ignored in input.
	if (OnRuneSlotSelected.IsBound()) {
		OnRuneSlotSelected.Broadcast(RuneSlotIdx);
	}
}
