// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Player/PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Characters/Human/Player/DefaultPlayerController.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/PawnStats.h"
#include "Components/Character/PlayerInteractionRaycast.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapon.h"
#include "Items/Clothes/RuneRing.h"
#include "UI/Wizard/PlayerHUD.h"

APlayerPawn::APlayerPawn(): Super()
{
	FP_RootComponent = CreateDefaultSubobject<USceneComponent>(FName("FP_Root"));
	FP_RootComponent->SetupAttachment(RootComponent);

	//Arms
	ArmsSpringComponent = CreateDefaultSubobject<USpringArmComponent>(FName("ArmsSpring"));
	ArmsSpringComponent->SetupAttachment(FP_RootComponent);

	ArmsOffsetComponent = CreateDefaultSubobject<USceneComponent>(FName("ArmsOffset"));
	ArmsOffsetComponent->SetupAttachment(ArmsSpringComponent);

	ArmsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMeshComponent");
	ArmsMeshComponent->SetupAttachment(ArmsOffsetComponent);
	ArmsMeshComponent->bCastDynamicShadow = false;
	ArmsMeshComponent->CastShadow = false;
	ArmsMeshComponent->SetOnlyOwnerSee(true);

	WeaponSocket = CreateDefaultSubobject<USceneComponent>(FName("WeaponSocket"));
	WeaponSocket->SetupAttachment(ArmsMeshComponent, FName("WeaponSocket"));
	
	//Camera
	CameraSpringComponent = CreateDefaultSubobject<USpringArmComponent>(FName("CameraSpring"));
	CameraSpringComponent->SetupAttachment(FP_RootComponent);

	CameraSkeletonComponent = CreateDefaultSubobject<USkeletalMeshComponent>("CameraSkeleton");
	CameraSkeletonComponent->SetupAttachment(CameraSpringComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(CameraSkeletonComponent, "Camera");

	PlayerInteraction = CreateDefaultSubobject<UPlayerInteractionRaycast>(FName("PlayerInteraction"));
	PlayerInteraction->SetupAttachment(CameraComponent);

	// Enable auto possession
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
		PlayerInteraction->OnItemPickedUp.AddDynamic(Inventory, &UInventoryComponent::AddItem);
		
		// Setup WizardHUD
		if (UPlayerHUD* PlayerHUD = PlayerController->AddHudToViewport())
		{
			PlayerHUD->Player = this;
			
			UE_LOG(LogTemp, Display, TEXT("Added WizardHUD to viewport."))

			PlayerController->OnToggleInventoryAction.AddDynamic(PlayerHUD, &UPlayerHUD::ToggleInventoryVisibility);
			PlayerInteraction->OnNewInteractionTarget.AddDynamic(PlayerHUD, &UPlayerHUD::OnNewInteractionTarget);
			
			// Bind stat changes to hud
			Stats->OnHurt.AddDynamic(PlayerHUD, &UPlayerHUD::OnPlayerHurt);
			Stats->OnHeal.AddDynamic(PlayerHUD, &UPlayerHUD::OnPlayerHeal);
			Stats->OnManaUsage.AddDynamic(PlayerHUD, &UPlayerHUD::OnPlayerManaUsage);
			Stats->Heal(0.f);
			Stats->UseMana(nullptr, 0.f);
		
			// PlayerInteraction->OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);
			
			// SpellBook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::BindRuneToSlot);
			// SpellBook->OnRuneCasted.AddDynamic(WizardHUD, &UWizardHUD::OnRuneCasted);
		}
		
		// Bind interaction system
		PlayerInteraction->OnItemPickedUp.AddDynamic(this, &APlayerPawn::UseItem);
		// PlayerInteraction->OnRunePickedUp.AddDynamic(SpellBook, &USpellbookComponent::AddRune);
		
		PlayerController->SetInputContext(EInputContextType::Movement, true);
		PlayerController->SetInputContext(EInputContextType::Interaction, true);
		PlayerController->SetInputContext(EInputContextType::UI, true);
	}
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerPawn::Interact()
{
	Super::Interact();

	AActor* InteractionTarget = PlayerInteraction->GetInteractionTarget();
	if (InteractionTarget == nullptr)
	{
		return;
	}

	if (AWeapon* NewWeapon = Cast<AWeapon>(InteractionTarget))
	{
		EquipWeapon(NewWeapon, ArmsMeshComponent, FName("WeaponSocket"));
	}
}

void APlayerPawn::SetArmor(AClothes* NewClothes, EArmorTarget ArmorTarget)
{
	if (ARuneRing* RuneRing = Cast<ARuneRing>(NewClothes))
	{
		
		return;
	}
	if (ArmorTarget == EArmorTarget::Hands)
	{
		
	}
	
	Super::SetArmor(NewClothes, ArmorTarget);
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
