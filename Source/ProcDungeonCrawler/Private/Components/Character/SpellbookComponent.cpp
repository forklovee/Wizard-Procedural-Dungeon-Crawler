// Fill out your copyright notice in the Description page of Project Settings.
#define ECC_SpellInteractive ECC_GameTraceChannel1


#include "Components/Character/SpellbookComponent.h"

#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "..\..\..\Public\Spell\Rune.h"
#include "..\..\..\Public\Spell\Spell.h"

USpellBookComponent::USpellBookComponent()
{
}

void USpellBookComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpellBookDataTable.IsNull()) return;
	
	const UDataTable* SpellBookLoaded = SpellBookDataTable.LoadSynchronous();
	
	TArray<FSpellCastRuneChain*> SpellBookRows;
	SpellBookLoaded->GetAllRows<FSpellCastRuneChain>(TEXT("Spell"), SpellBookRows);
	
	for (const FSpellCastRuneChain* Row : SpellBookRows)
	{
		TArray<URune*> RuneChain;
		for (const TSoftObjectPtr<URune>& RuneCast : Row->RuneChain)
		{
			if (RuneCast.IsNull()) continue;
			
			RuneChain.Add(RuneCast.LoadSynchronous());
		}
		SpellBook.Add(RuneChain, Row->SpellCast);
	}
}

void USpellBookComponent::AddRune(URune* NewRuneCast)
{
	if (Runes.Contains(NewRuneCast)) return;
	
	const int RuneIdx = Runes.Add(NewRuneCast);

	OnRuneAdded.Broadcast(RuneIdx ,NewRuneCast);
}

URune* USpellBookComponent::GetRuneOfIdx(int Idx)
{
	if (Idx < 0 || Idx > Runes.Num() - 1) {
		return nullptr;
	}
	return Runes[Idx];
}

void USpellBookComponent::CastRune(URune* RuneCast)
{
	if (CastedRunes.Num() >= 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Casted runes quantity excessed"));
		return;
	}
	
	// Set auto clear timer
	GetWorld()->GetTimerManager().SetTimer(ClearCastedRunesTimerHandle, this, &USpellBookComponent::ClearCastedRunes, 2.0, false);
	
	CastedRunes.Push(RuneCast);
	if (OnRuneCasted.IsBound())
	{
		OnRuneCasted.Broadcast(Runes.Find(RuneCast), RuneCast, CastedRunes);
	}
	
	float OutManaCost = 0.f;
	if (const TSubclassOf<ASpell> SpellCastClass = GetSpellCastClass(CastedRunes, OutManaCost))
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

float USpellBookComponent::GetRuneSequenceManaCost(TArray<URune*>& RuneSequence)
{
	float ManaCost = 0.f;
	for (const URune* RuneCast: RuneSequence)
	{
		ManaCost += RuneCast->ManaCost;
	}
	return ManaCost;
}

void USpellBookComponent::CastRuneOfIdx(int Idx)
{ 
	if (URune* RuneCast = GetRuneOfIdx(Idx))
	{
		CastRune(RuneCast);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Rune of index %d not found"), Idx);
	}
}

TSubclassOf<ASpell> USpellBookComponent::GetSpellCastClass(TArray<URune*>& RuneSequence, float& OutManaCost) const
{
	TArray< TArray<URune*> > RuneChains;
	SpellBook.GetKeys(RuneChains);
	
	const int RuneChainId = RuneChains.Find(RuneSequence);

	if (RuneChainId != INDEX_NONE) //Spell found
	{
		OutManaCost = GetRuneSequenceManaCost(RuneSequence);
		return SpellBook[RuneSequence];
	}
	
	return nullptr;
}

TArray<URune*> USpellBookComponent::GetRequiredRunesForSpell(TSubclassOf<ASpell> SpellCastClass) const
{
	TArray<TSubclassOf<ASpell>> SpellCastClasses;
	SpellBook.GenerateValueArray(SpellCastClasses);
	const int SpellId = SpellCastClasses.Find(SpellCastClass);
	if (SpellId == INDEX_NONE)
	{
		return {};
	}

	TArray<TArray<URune*>> RuneChains;
	SpellBook.GenerateKeyArray(RuneChains);
	return RuneChains[SpellId];
}

bool USpellBookComponent::CanSpellBeCasted(TSubclassOf<ASpell> SpellCastClass,
	TArray<URune*>& OutMissingRunes) const
{
	const TArray<URune*> RequiredRunes = GetRequiredRunesForSpell(SpellCastClass);
	if (RequiredRunes.Num() == 0)
	{
		return false;
	}

	bool bCanCast = true;
	for (URune* RequiredRune : RequiredRunes)
	{
		if (!Runes.Contains(RequiredRune))
		{
			bCanCast = false;
			OutMissingRunes.Add(RequiredRune);
		}
	}
	return bCanCast;
}

bool USpellBookComponent::IsSpellPrepared() const
{
	return PreparedSpell.Key.IsValid();
}

void USpellBookComponent::PrepareSpell(TSubclassOf<ASpell> SpellCastClass, float ManaCost)
{
	UE_LOG(LogTemp, Warning, TEXT("Spell prepared: %s"), *SpellCastClass->GetName());
	
	// if (ASpell* SpellToCast = Cast<ASpell>(GetWorld()->SpawnActor(SpellCastClass, &SpawnLocation)))
	// {
	// 	// Todo attach spell to character
	// 	// SpellToCast->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	// 	// Spell spawned successfully
	// 	PreparedSpell.Key = SpellToCast;
	// 	PreparedSpell.Value = ManaCost;
	//
	// 	SetComponentTickEnabled(true);
	// }
}

void USpellBookComponent::CastPreparedSpell(AHuman* Human)
{
	if (!IsSpellPrepared()) return;

	ASpell* SpellCast = PreparedSpell.Key.Get();
	const float ManaCost = PreparedSpell.Value;
	
	SpellCast->CastSpell(Human);
	
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

void USpellBookComponent::ClearCastedRunes()
{
	CastedRunes = TArray<URune*>();

	if (OnCastedRunesCleared.IsBound())
	{
		OnCastedRunesCleared.Broadcast();
	}
}
