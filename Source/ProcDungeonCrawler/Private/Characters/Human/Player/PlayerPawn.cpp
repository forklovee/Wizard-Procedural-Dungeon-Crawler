// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Human/Player/PlayerPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Characters/Human/Player/DefaultPlayerController.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/PawnStats.h"
#include "Components/Character/PlayerInteractionRaycast.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapon.h"
#include "Items/Clothes/RuneRing.h"
#include "UI/Wizard/PlayerHUD.h"

APlayerPawn::APlayerPawn()
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
	AutoReceiveInput = EAutoReceiveInput::Player0;

	PrimaryActorTick.bCanEverTick = true;
}

void APlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PlayerInteraction->UpdateInteractionTarget(CameraComponent->GetForwardVector());
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(GetController());
	
	// set ui inputs
	PlayerController->SetupDefaultInput(Cast<UEnhancedInputComponent>(PlayerInputComponent));
	
	PlayerController->SetInputContext(EInputContextType::Movement, true);
	PlayerController->SetInputContext(EInputContextType::Interaction, true);
	// PlayerController->SetInputContext(EInputContextType::UI, true);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(GetController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get player controller"));
		return;
	}
	
	// Movement context
	PlayerController->OnMoveAroundAction.AddDynamic(this, &APlayerPawn::MoveAround);
	PlayerController->OnLookAroundAction.AddDynamic(this, &APlayerPawn::LookAround);
	PlayerController->OnSprintToggledAction.AddDynamic(this, &APlayerPawn::SetSprinting);
	PlayerController->OnCrouchToggledAction.AddDynamic(this, &APlayerPawn::ToggleCrouch);
	PlayerController->OnJumpAction.AddDynamic(this, &APlayerPawn::Jump);

	// Interaction context
	PlayerController->OnPrimaryAction.AddDynamic(this, &APlayerPawn::PrimaryAction);
	PlayerController->OnSecondaryAction.AddDynamic(this, &APlayerPawn::SecondaryAction);
	PlayerController->OnInteractAction.AddDynamic(PlayerInteraction, &UPlayerInteractionRaycast::Interact);

	Inventory->OnWeaponEquipped.AddDynamic(this, &APlayerPawn::SetWeaponActor);
	Inventory->OnWeaponUnEquipped.AddDynamic(this, &APlayerPawn::SetWeaponActor);
	
	// Setup HUD
	PlayerHUD = PlayerController->AddHudToViewport();
	if (PlayerHUD.IsValid())
	{
		PlayerHUD->SetupInventory(this);
		
		UE_LOG(LogTemp, Display, TEXT("Added WizardHUD to viewport."))

		PlayerController->OnToggleInventoryAction.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::ToggleInventoryVisibility);
		PlayerHUD->OnInventoryVisibilityChanged.AddDynamic(this, &APlayerPawn::UpdateInventoryInputContext);
		
		PlayerInteraction->OnNewInteractionTarget.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::OnNewInteractionTarget);
		
		PlayerInteraction->OnItemPickedUp.AddDynamic(Inventory, &UInventoryComponent::AddItem);
		Inventory->OnItemAdded.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::UpdateInventorySlot);
		Inventory->OnItemRemoved.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::UpdateInventorySlot);

		// Bind stat changes to hud
		Stats->OnHurt.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::UpdateHealthBar);
		Stats->OnHeal.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::UpdateHealthBar);
		Stats->OnManaUsage.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::UpdateManaBar);
		Stats->OnManaRestore.AddDynamic(PlayerHUD.Get(), &UPlayerHUD::UpdateManaBar);
		PlayerHUD->UpdateHealthBar(Stats->GetHealth(), Stats->GetMaxHealth());
		PlayerHUD->UpdateManaBar(Stats->GetMana(), Stats->GetMaxMana());
		
		// PlayerInteraction->OnNewInteractionTarget.AddDynamic(WizardHUD->InteractionUI, &UInteractionUI::UpdateInteractionPrompt);
		
		// SpellBook->OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::BindRuneToSlot);
		// SpellBook->OnRuneCasted.AddDynamic(WizardHUD, &UWizardHUD::OnRuneCasted);
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

void APlayerPawn::SetWeaponActor(AWeapon* NewWeapon, FInventorySlot& InventorySlot)
{
	UE_LOG(LogTemp, Display, TEXT("Set new weapon Dupa"))
	if (NewWeapon == nullptr) return;
	
	NewWeapon->Equip(this, ArmsMeshComponent, FName("WeaponSocket"));
}

void APlayerPawn::UpdateInventoryInputContext(bool bIsInventoryOpen)
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(GetController());
	PlayerController->SetInputContext(EInputContextType::Movement, !bIsInventoryOpen);
	PlayerController->SetInputContext(EInputContextType::Interaction, !bIsInventoryOpen);
	PlayerController->SetInputContext(EInputContextType::UI, bIsInventoryOpen);
}

// void APlayerPawn::OnRuneSlotKeyPressed(const FInputActionValue& Value)
// {
// 	const int32 RuneSlotIdx = ((int32)Value.Get<float>()) - 1; // "1 key" is equal to scalar of 1 because 0 is ignored in input.
// 	if (OnRuneSlotSelected.IsBound()) {
// 		OnRuneSlotSelected.Broadcast(RuneSlotIdx);
// 	}
// }
