// Fill out your copyright notice in the Description page of Project Settings.

#include "WizardCharacter.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/PropPickupInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/WizardPlayerController.h"
#include "Spell/RunePickup.h"
#include "Spell/RuneCast.h"
#include "UI/Wizard/WizardHUD.h"

#include "Engine/DataTable.h"
#include "Spell/SpellCast.h"

AWizardCharacter::AWizardCharacter()
{
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->bUsePawnControlRotation = true;
	
	ArmsMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("HandsMeshComponent");
	ArmsMeshComponent->SetupAttachment(CameraComponent, NAME_None);
	ArmsMeshComponent->bCastDynamicShadow = false;
	ArmsMeshComponent->CastShadow = false;
	ArmsMeshComponent->SetOnlyOwnerSee(true);
	
}

void AWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementSpeed = WalkingSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Adding Dynamic shit!"))

	if (AWizardPlayerController* WizardPlayerController = Cast<AWizardPlayerController>(GetController()))
	{
		if (UWizardHUD* WizardHUD = WizardPlayerController->SetupWizardHud())
		{
			OnRuneAdded.AddDynamic(WizardHUD, &UWizardHUD::AddRuneToUI);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fucking shit!"))
		}
	}

	SetSpellDataFromDataTable();
}

void AWizardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		CameraComponent->GetComponentLocation(),
		CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * 1000,
		100,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{},
		EDrawDebugTrace::ForOneFrame,
		LookingAt_HitActors,
		true
	);

	// UE_LOG(LogTemp, Warning, TEXT("Looking at %i actors."), LookingAt_HitActors.Num());
	// if (LookingAt_HitActors.Num() > 0)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("First actor %s"), *LookingAt_HitActors[0].GetActor()->GetName());
	// }
	
}

void AWizardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Set action inputs
	Input->BindAction(LookAround_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::LookAround);
	Input->BindAction(Movement_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::MoveAround);
	Input->BindAction(Sprint_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ToggleSprint);
	Input->BindAction(Jump_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::Jump);

	// set magic inputs
	Input->BindAction(RuneCast_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::CastRune);
	// Input->BindAction(ChangeSpellSlot_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::ChangeSpellSlot);
	
	// set interaction inputs
	Input->BindAction(PrimaryAction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::PrimaryAction);
	// Input->BindAction(Interaction_InputAction.Get(), ETriggerEvent::Triggered, this, &AWizardCharacter::PrimaryAction);
	
	// Set action context
	const ULocalPlayer* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetLocalPlayer();
	if (!InputMappingContext.IsNull())
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			EnhancedInputSubsystem->AddMappingContext(InputMappingContext.LoadSynchronous(), 0);

			UE_LOG(LogTemp, Warning, TEXT("Mapping context added for %s"), *EnhancedInputSubsystem->GetLocalPlayer()->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't get enhanced input subsystem subsystem!"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid mapping context!"))
	}
}

bool AWizardCharacter::AddRune(TSoftObjectPtr<URuneCast> NewRuneCast)
{
	UE_LOG(LogTemp, Warning, TEXT("b4, broadcast"))
	if (NewRuneCast.IsNull()) return false;
	UE_LOG(LogTemp, Warning, TEXT("not null!!!"))
	if (Runes.Contains(NewRuneCast)) return false;
	
	Runes.Add(NewRuneCast);

	UE_LOG(LogTemp, Warning, TEXT("broadcast"))
	OnRuneAdded.Broadcast(NewRuneCast.LoadSynchronous());
	
	return true;
}

void AWizardCharacter::MoveAround(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Axis2D)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}
	
	const FVector2D MovementInput = Value.Get<FVector2D>();
	if (Controller == nullptr) return;
	
	AddMovementInput(GetActorRightVector(), MovementInput.Y);
	AddMovementInput(GetActorForwardVector(), MovementInput.X);
}

void AWizardCharacter::LookAround(const FInputActionValue& Value)
{
	if (bIsCastingSpell) return;
	
	if (Value.GetValueType() != EInputActionValueType::Axis2D)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}

	const FVector2D MouseMovementVector = Value.Get<FVector2D>();
	
	if (Controller == nullptr) return;

	AddControllerYawInput(MouseMovementVector.X);
	AddControllerPitchInput(-MouseMovementVector.Y);
}

void AWizardCharacter::ToggleSprint(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Boolean)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}
	bIsSprinting = Value.Get<bool>();
	
	MovementSpeed = bIsSprinting ? RunningSpeed : WalkingSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void AWizardCharacter::CastRune(const FInputActionValue& Value)
{
	if (CastedRunes.Num() >= 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Casted runes quantity excessed"));
		return;
	}
	
	const int RuneId = static_cast<int>(Value.Get<float>() - 1.0);

	if (RuneId < 0 || RuneId >= Runes.Num()) return;

	TSoftObjectPtr<URuneCast> RuneToCast = Runes[RuneId];
	if (RuneToCast == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Rune of id %i is null"), RuneId);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Cast Rune: %s"), *RuneToCast->GetName());

	// Set auto clear timer
	GetWorld()->GetTimerManager().SetTimer(ClearCastedRunesTimerHandle, this, &AWizardCharacter::ClearCastedRunes, 2.0, false);
	
	CastedRunes.Push(RuneToCast);
	
	if (IsRuneSequenceValid(CastedRunes))
	{
		
	}
	else
	{
		if (CastedRunes.Num() == 4)
		{
			ClearCastedRunes();
			GetWorld()->GetTimerManager().ClearTimer(ClearCastedRunesTimerHandle);
		}
	}
}

bool AWizardCharacter::IsRuneSequenceValid(TArray<TSoftObjectPtr<URuneCast>> SpellRunes, TSubclassOf<ASpellCast>& OutSpellCastClass)
{
	const int RuneChainId = RuneChains.Find(SpellRunes);
	UE_LOG(LogTemp, Warning, TEXT("Found spell id: %i"), RuneChainId);

	if (RuneChainId != INDEX_NONE && RuneChainId < SpellCastClasses.Num()) //Spell found
	{
		OutSpellCastClass = SpellCastClasses[RuneChainId];

		ClearCastedRunes();
		GetWorld()->GetTimerManager().ClearTimer(ClearCastedRunesTimerHandle);
		return true;
	}
	
	if (CastedRunes.Num() == 4)
	{
		ClearCastedRunes();
		GetWorld()->GetTimerManager().ClearTimer(ClearCastedRunesTimerHandle);
	}
	return false;
}

bool AWizardCharacter::IsSpellPreparationStateValid()
{
	
}

void AWizardCharacter::PrepareSpell()
{
	
}

void AWizardCharacter::CastSpell()
{
	CastSpell(CastedRunes);
}

void AWizardCharacter::CastSpell(TArray<TSoftObjectPtr<URuneCast>> SpellRunes)
{
	const int RuneChainId = RuneChains.Find(SpellRunes);
	UE_LOG(LogTemp, Warning, TEXT("Found spell id: %i"), RuneChainId);

	if (RuneChainId != INDEX_NONE && RuneChainId < SpellCastClasses.Num()) //Spell found
	{
		const TSubclassOf<ASpellCast> SpellCastClass = SpellCastClasses[RuneChainId];
		const FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
		if (ASpellCast* SpellToCast = Cast<ASpellCast>(GetWorld()->SpawnActor(SpellCastClass, &SpawnLocation)))
		{
			CastedSpell = SpellToCast;
			if (CastedSpell.IsValid())
			{
				if (CastedSpell->bRequireTarget)
				{
					CastedSpell->CastSpell(this);
				}
			}
			else
			{
				CastedSpell = nullptr;
			}
		}

		ClearCastedRunes();
		GetWorld()->GetTimerManager().ClearTimer(ClearCastedRunesTimerHandle);
		return;
	}
	
	if (CastedRunes.Num() == 4)
	{
		ClearCastedRunes();
		GetWorld()->GetTimerManager().ClearTimer(ClearCastedRunesTimerHandle);
	}
}

void AWizardCharacter::ClearCastedRunes()
{
	CastedRunes = TArray<TSoftObjectPtr<URuneCast>>();
}

void AWizardCharacter::PrimaryAction(const FInputActionValue& Value)
{
	if (Value.GetValueType() != EInputActionValueType::Boolean)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid input value in %s"), *FString(__FUNCTION__));
		return;
	}

	if (LookingAt_HitActors.Num() == 0)
	{
		if (CastedSpell.IsValid())
		{
			CastedSpell->Destroy();
		}
		CastedSpell = nullptr;
		return;
	}
	
	AActor* FirstActor = LookingAt_HitActors[0].GetActor();

	if (!FirstActor->Implements<UPropPickupInterface>())
	{
		return;
	}

	AActor* PickedUpActor = IPropPickupInterface::Execute_Pickup(FirstActor, this);

	// Add rune if rune pickup
	if (ARunePickup* RunePickup = Cast<ARunePickup>(PickedUpActor))
	{
		AddRune(RunePickup->RuneCast);
	}

	// Remove actor
	PickedUpActor->Destroy();
}

void AWizardCharacter::SetSpellDataFromDataTable()
{
	if (SpellBookDataTable.IsNull()) return;
	
	const UDataTable* SpellBookLoaded = SpellBookDataTable.LoadSynchronous();
	
	TArray<FSpellCastRuneChain*> SpellBookRows;
	SpellBookLoaded->GetAllRows<FSpellCastRuneChain>(TEXT("Spell"), SpellBookRows);
	
	for (const FSpellCastRuneChain* Row : SpellBookRows)
	{
		SpellCastClasses.Add(Row->SpellCast);
		RuneChains.Add(Row->RuneChain);
	}
}

