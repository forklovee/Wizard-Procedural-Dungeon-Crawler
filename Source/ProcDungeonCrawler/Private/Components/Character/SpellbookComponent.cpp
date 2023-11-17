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

	if (PreparedSpell.Key.IsValid())
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
	
	float OutManaCost = 0.f;
	if (const TSubclassOf<ASpellCast> SpellCastClass = GetSpellCastClass(CastedRunes, OutManaCost))
	{
		if (OnValidRuneSequenceCasted.IsBound())
		{
			OnValidRuneSequenceCasted.Broadcast(SpellCastClass, OutManaCost);
		}
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

float USpellbookComponent::GetRuneSequenceManaCost(TArray<URuneCast*>& RuneSequence)
{
	float ManaCost = 0.f;
	for (const URuneCast* RuneCast: RuneSequence)
	{
		ManaCost += RuneCast->ManaCost;
	}
	return ManaCost;
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

TSubclassOf<ASpellCast> USpellbookComponent::GetSpellCastClass(TArray<URuneCast*>& RuneSequence, float& OutManaCost) const
{
	TArray< TArray<URuneCast*> > RuneChains;
	SpellBook.GetKeys(RuneChains);
	
	const int RuneChainId = RuneChains.Find(RuneSequence);

	if (RuneChainId != INDEX_NONE) //Spell found
	{
		OutManaCost = GetRuneSequenceManaCost(RuneSequence);
		return SpellBook[RuneSequence];
	}
	
	return nullptr;
}

bool USpellbookComponent::IsSpellPrepared() const
{
	return PreparedSpell.Key.IsValid();
}

void USpellbookComponent::PrepareSpell(TSubclassOf<ASpellCast> SpellCastClass, float ManaCost)
{
	UE_LOG(LogTemp, Warning, TEXT("Spell prepared: %s"), *SpellCastClass->GetName());

	const FVector SpawnLocation = GetComponentLocation();
	if (ASpellCast* SpellToCast = Cast<ASpellCast>(GetWorld()->SpawnActor(SpellCastClass, &SpawnLocation)))
	{
		SpellToCast->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		// Spell spawned successfully
		PreparedSpell.Key = SpellToCast;
		PreparedSpell.Value = ManaCost;

		SetComponentTickEnabled(true);
	}
}

void USpellbookComponent::CastPreparedSpell(AWizardCharacter* WizardCharacter)
{
	if (!IsSpellPrepared()) return;

	ASpellCast* SpellCast = PreparedSpell.Key.Get();
	const float ManaCost = PreparedSpell.Value;
	
	SpellCast->CastSpell(WizardCharacter);
	
	if (SpellCast->bRequireTarget && SpellTargets.Num() > 0)
	{
		if (AActor* TargetActor = SpellCast->GetFirstValidHitResultActor(SpellTargets))
		{
			SpellCast->TargetActor(TargetActor);
			SpellCast->ApplyEffectsOnTarget(TargetActor);
		}
	}

	if (OnSpellCasted.IsBound())
	{
		OnSpellCasted.Broadcast(SpellCast, ManaCost);
	}
	
	PreparedSpell.Key = nullptr;
	PreparedSpell.Value = 0.f;
}

void USpellbookComponent::ClearCastedRunes()
{
	CastedRunes = TArray<URuneCast*>();

	if (OnCastedRunesCleared.IsBound())
	{
		OnCastedRunesCleared.Broadcast();
	}
}
