// Fill out your copyright notice in the Description page of Project Settings.
#define ECC_SpellInteractive ECC_GameTraceChannel1


#include "Components/Character/SpellbookComponent.h"

#include "InputActionValue.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spell/RuneCast.h"
#include "Spell/SpellCast.h"

USpellbookComponent::USpellbookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpellbookComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	
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


void USpellbookComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PreparedSpell != nullptr)
	{
		UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(),
			GetComponentLocation(),
			GetComponentLocation() + GetForwardVector() * 500.0,
			100,
			UEngineTypes::ConvertToTraceType(ECC_SpellInteractive),
			false,
			{},
			EDrawDebugTrace::ForOneFrame,
			SpellTargets,
			true
		);
	}
}

bool USpellbookComponent::AddRune(TSoftObjectPtr<URuneCast> NewRuneCast)
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

void USpellbookComponent::CastRune(const FInputActionValue& Value)
{
	if (CastedRunes.Num() >= 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Casted runes quantity excessed"));
		return;
	}
	
	const int RuneId = static_cast<int>(Value.Get<float>() - 1.0);

	if (RuneId < 0 || RuneId >= Runes.Num()) return;

	const TSoftObjectPtr<URuneCast> RuneToCast = Runes[RuneId];
	if (RuneToCast == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Rune of id %i is null"), RuneId);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Cast Rune: %s"), *RuneToCast->GetName());

	// Set auto clear timer
	GetWorld()->GetTimerManager().SetTimer(ClearCastedRunesTimerHandle, this, &USpellbookComponent::ClearCastedRunes, 2.0, false);
	
	CastedRunes.Push(RuneToCast);
	TSubclassOf<ASpellCast> OutSpellCastClass;
	if (IsRuneSequenceValid(CastedRunes, OutSpellCastClass))
	{
		PrepareSpell(OutSpellCastClass);
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

bool USpellbookComponent::IsRuneSequenceValid(TArray<TSoftObjectPtr<URuneCast>> SpellRunes, TSubclassOf<ASpellCast>& OutSpellCastClass)
{
	const int RuneChainId = RuneChains.Find(SpellRunes);

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

bool USpellbookComponent::IsSpellPrepared() const
{
	return PreparedSpell.IsValid();
}

ASpellCast* USpellbookComponent::GetPreparedSpell() const
{
	return PreparedSpell.Get();
}

void USpellbookComponent::PrepareSpell(TSubclassOf<ASpellCast> SpellCastClass)
{
	UE_LOG(LogTemp, Warning, TEXT("Spell prepared: %s"), *SpellCastClass->GetName());

	const FVector SpawnLocation = GetComponentLocation();
	if (ASpellCast* SpellToCast = Cast<ASpellCast>(GetWorld()->SpawnActor(SpellCastClass, &SpawnLocation)))
	{
		SpellToCast->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		// Spell spawned successfully
		PreparedSpell = SpellToCast;

		SetComponentTickEnabled(true);
	}
}

void USpellbookComponent::CastPreparedSpell(AWizardCharacter* WizardCharacter)
{
	if (!IsSpellPrepared()) return;

	PreparedSpell->CastSpell(WizardCharacter);
	if (PreparedSpell->bRequireTarget && SpellTargets.Num() > 0)
	{
		if (AActor* TargetActor = PreparedSpell->GetFirstValidHitResultActor(SpellTargets))
		{
			PreparedSpell->TargetActor(TargetActor);
			PreparedSpell->ApplyEffectsOnTarget(TargetActor);
		}
	}
	PreparedSpell = nullptr;
}

void USpellbookComponent::ClearCastedRunes()
{
	CastedRunes = TArray<TSoftObjectPtr<URuneCast>>();
}
