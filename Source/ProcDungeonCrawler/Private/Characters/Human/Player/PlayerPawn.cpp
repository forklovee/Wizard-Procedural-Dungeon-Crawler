// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Player/PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/Human/Player/DefaultPlayerController.h"
#include "Components/Character/PlayerInteractionRaycast.h"
#include "GameFramework/SpringArmComponent.h"

APlayerPawn::APlayerPawn(): Super()
{
	CameraArmComponent = CreateDefaultSubobject<USpringArmComponent>("CameraArmComponent");
	CameraArmComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(CameraArmComponent);
	
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

void APlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PlayerInteraction->UpdateInteractionTarget(CameraComponent->GetForwardVector());
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerEnhancedInputComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't cast PlayerInputComponent to UEnhancedInputComponent."));
		return;
	}
	
	// set ui inputs
	if (ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(GetController()))
	{
		PlayerController->SetupDefaultInput(PlayerEnhancedInputComponent);
		
		PlayerController->OnMoveAroundInput.AddDynamic(this, &APlayerPawn::MoveAround);
		PlayerController->OnLookAroundInput.AddDynamic(this, &APlayerPawn::LookAround);
		
		// Setup WizardHUD
		// if (UWizardHUD* WizardHUD = PlayerController->GetWizardHud())
		// {
		// 	// Bind stat changes to hud
		// 	Stats->OnHurt.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerHurt);
		// 	Stats->OnHeal.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerHeal);
		// 	Stats->OnManaUsage.AddDynamic(WizardHUD, &UWizardHUD::OnPlayerManaUsage);
		// 	Stats->Heal(0.f);
		// 	Stats->UseMana(nullptr, 0.f);
		//
		// 	PlayerInteraction->OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);
		// 	
		// 	SpellBook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::BindRuneToSlot);
		// 	SpellBook->OnRuneCasted.AddDynamic(WizardHUD, &UWizardHUD::OnRuneCasted);
		// }
		// Bind interaction system
		PlayerInteraction->OnItemPickedUp.AddDynamic(this, &APlayerPawn::UseItem);
		// PlayerInteraction->OnRunePickedUp.AddDynamic(SpellBook, &USpellbookComponent::AddRune);
		
		PlayerController->SetInputContext(EInputContextType::Gameplay, true);
	}
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}
//
// void APlayerPawn::UpdateSpellBookInputContext(bool IsSpellBookOpen)
// {
// 	//TODO: Add Bag specific actions
//
// 	SetInteractionInput(!IsSpellBookOpen);
// 	SetCharacterMovementInput(!IsSpellBookOpen);
// }
//
// void APlayerPawn::UpdateMapInputContext(bool IsMapOpenOpen)
// {
// 	//TODO: Add Bag specific actions
//
// 	SetInteractionInput(!IsMapOpenOpen);
// 	SetCharacterMovementInput(!IsMapOpenOpen);
// }
//
// void APlayerPawn::OnInteractAction(const FInputActionValue& Value)
// {
// 	if (PlayerInteraction->IsGrabbingItem() || PlayerInteraction->bInteractionDisabled)
// 	{
// 		return;
// 	}
// 	
// 	bool bInteractionState = PlayerInteraction->Interact();
// }
//
// void APlayerPawn::OnMoveAroundAction(const FInputActionValue& Value)
// {
// 	const FVector2D MoveDirection = Value.Get<FVector2D>();
// 	MoveAround(MoveDirection);
// }
//
// void APlayerPawn::OnLookAroundAction(const FInputActionValue& Value)
// {
// 	LookAround(Value.Get<FVector2D>());
// }
//
// void APlayerPawn::OnSetSprintAction(const FInputActionValue& Value)
// {
// 	SetSprinting(Value.Get<bool>());
// }
//
// void APlayerPawn::OnSetCrouchAction(const FInputActionValue& Value)
// {
// 	SetCrouch(!IsCrouching());
// }
//
// void APlayerPawn::OnToggleBagAction(const FInputActionValue& Value)
// {
// 	if (OnToggleBagRequest.IsBound())
// 	{
// 		const bool IsBagOpen = Bag->IsOpen();
// 		SetCrouch(!IsBagOpen);
// 		if (IsBagOpen)
// 		{
// 			OnUILeftRightInput.AddDynamic(Bag, &UBagComponent::OnLeftRightInputAction);
// 		}
// 		else
// 		{
// 			OnUILeftRightInput.RemoveDynamic(Bag, &UBagComponent::OnLeftRightInputAction);
// 		}
// 		OnToggleBagRequest.Broadcast(!IsBagOpen);
// 	}
// }
//
// void APlayerPawn::OnRuneSlotKeyPressed(const FInputActionValue& Value)
// {
// 	const int32 RuneSlotIdx = ((int32)Value.Get<float>()) - 1; // "1 key" is equal to scalar of 1 because 0 is ignored in input.
// 	if (OnRuneSlotSelected.IsBound()) {
// 		OnRuneSlotSelected.Broadcast(RuneSlotIdx);
// 	}
// }
