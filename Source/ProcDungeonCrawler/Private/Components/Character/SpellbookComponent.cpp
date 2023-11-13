// Fill out your copyright notice in the Description page of Project Settings.
#define ECC_SpellInteractive ECC_GameTraceChannel1


#include "Components/Character/SpellbookComponent.h"

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
		TArray<URuneCast*> RuneChain;
		for (const TSoftObjectPtr<URuneCast>& RuneCast : Row->RuneChain)
		{
			if (RuneCast.IsNull()) continue;
			
			RuneChain.Add(RuneCast.LoadSynchronous());
		}
		SpellBook.Add(RuneChain, Row->SpellCast);
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
			EDrawDebugTrace::None,
			SpellTargets,
			true
		);
	}
}

void USpellbookComponent::AddRune(URuneCast* NewRuneCast)
{
	if (Runes.Contains(NewRuneCast)) return;
	
	const int RuneIdx = Runes.Add(NewRuneCast);

	OnRuneAdded.Broadcast(RuneIdx ,NewRuneCast);
}

URuneCast* USpellbookComponent::GetRuneOfIdx(int Idx)
{
	if (Idx < 0 || Idx > Runes.Num() - 1) {
		return nullptr;
	}
	return Runes[Idx];
}

void USpellbookComponent::CastRune(URuneCast* RuneCast)
{
	if (CastedRunes.Num() >= 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Casted runes quantity excessed"));
		return;
	}
	
	// Set auto clear timer
	GetWorld()->GetTimerManager().SetTimer(ClearCastedRunesTimerHandle, this, &USpellbookComponent::ClearCastedRunes, 2.0, false);
	
	CastedRunes.Push(RuneCast);
	if (OnRuneCasted.IsBound())
	{
		OnRuneCasted.Broadcast(Runes.Find(RuneCast), RuneCast, CastedRunes);
	}
	
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

void USpellbookComponent::CastRuneOfIdx(int Idx)
{
	if (URuneCast* RuneCast = GetRuneOfIdx(Idx))
	{
		CastRune(RuneCast);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Rune of index %d not found"), Idx);
	}
}

bool USpellbookComponent::IsRuneSequenceValid(TArray<URuneCast*>& SpellRunes, TSubclassOf<ASpellCast>& OutSpellCastClass)
{
	TArray< TArray<URuneCast*> > RuneChains;
	SpellBook.GetKeys(RuneChains);
	
	const int RuneChainId = RuneChains.Find(SpellRunes);

	if (RuneChainId != INDEX_NONE) //Spell found
	{
		OutSpellCastClass = SpellBook[SpellRunes];

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
	//TODO: Check if spell can be casted
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
	CastedRunes = TArray<URuneCast*>();

	if (OnCastedRunesCleared.IsBound())
	{
		OnCastedRunesCleared.Broadcast();
	}
}
