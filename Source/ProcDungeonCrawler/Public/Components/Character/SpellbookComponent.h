// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellbookComponent.generated.h"

class AHuman;
class URune;
class ASpell;

struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRuneAdded, int, RuneIdx, URune*, RuneCast);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRuneCasted, int, RuneIdx, URune*, RuneCast, TArray<URune*>&, CastedRunes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValidRuneSequenceCasted, TSubclassOf<ASpell>, SpellCast, float, ManaCost);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellCasted, ASpell*, SpellCast, float, ManaCost);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCastedRunesCleared);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API USpellBookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnRuneAdded OnRuneAdded;
	FOnRuneCasted OnRuneCasted;
	FOnValidRuneSequenceCasted OnValidRuneSequenceCasted;
	
	FOnCastedRunesCleared OnCastedRunesCleared;
	
	FOnSpellCasted OnSpellCasted;
	
	USpellBookComponent();

	// Runes
	UFUNCTION()
	void AddRune(URune* NewRuneCast);

	URune* GetRuneOfIdx(int Idx);
	void CastRune(URune* RuneCast);
	UFUNCTION()
	void CastRuneOfIdx(int Idx);
	
	TSubclassOf<ASpell> GetSpellCastClass(TArray<URune*>& RuneSequence, float& OutManaCost) const;
	TArray<URune*> GetRequiredRunesForSpell(TSubclassOf<ASpell> SpellCastClass) const;
	bool CanSpellBeCasted(TSubclassOf<ASpell> SpellCastClass, TArray<URune*>& OutMissingRunes) const;
	
	void ClearCastedRunes();
	
	// Spells
	bool IsSpellPrepared() const;
	void PrepareSpell(TSubclassOf<ASpell> SpellCastClass, float ManaCost);

	void CastPreparedSpell(AHuman* Human);

	static float GetRuneSequenceManaCost(TArray<URune*>& RuneSequence);
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category= "Spells")
	TSoftObjectPtr<class UDataTable> SpellBookDataTable;

private:
	// Spell oriented variables
	TTuple<TWeakObjectPtr<ASpell>, float> PreparedSpell;
	TArray<FHitResult> SpellTargets;
	
	// Obtained Runes
	TArray<URune*> Runes;

	// Runes casted
	TArray<URune*> CastedRunes;
	FTimerHandle ClearCastedRunesTimerHandle;

	TMap<TArray<URune*>, TSubclassOf<ASpell>> SpellBook;
	
	bool bIsCastingSpell = false;
};
