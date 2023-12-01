// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellbookComponent.generated.h"

class AWizardCharacter;
class URuneCast;
struct FInputActionValue;
class ASpellCast;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRuneAdded, int, RuneIdx, URuneCast*, RuneCast);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRuneCasted, int, RuneIdx, URuneCast*, RuneCast, TArray<URuneCast*>&, CastedRunes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValidRuneSequenceCasted, TSubclassOf<ASpellCast>, SpellCast, float, ManaCost);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellCasted, ASpellCast*, SpellCast, float, ManaCost);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCastedRunesCleared);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API USpellbookComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FOnRuneAdded OnRuneAdded;
	FOnRuneCasted OnRuneCasted;
	FOnValidRuneSequenceCasted OnValidRuneSequenceCasted;
	
	FOnCastedRunesCleared OnCastedRunesCleared;
	
	FOnSpellCasted OnSpellCasted;
	
	USpellbookComponent();

	// Runes
	UFUNCTION()
	void AddRune(URuneCast* NewRuneCast);

	URuneCast* GetRuneOfIdx(int Idx);
	void CastRune(URuneCast* RuneCast);
	UFUNCTION()
	void CastRuneOfIdx(int Idx);
	
	TSubclassOf<ASpellCast> GetSpellCastClass(TArray<URuneCast*>& RuneSequence, float& OutManaCost) const;
	TArray<URuneCast*> GetRequiredRunesForSpell(TSubclassOf<ASpellCast> SpellCastClass) const;
	bool CanSpellBeCasted(TSubclassOf<ASpellCast> SpellCastClass, TArray<URuneCast*>& OutMissingRunes) const;
	
	void ClearCastedRunes();
	
	// Spells
	bool IsSpellPrepared() const;
	ASpellCast* GetPreparedSpell(float& OutManaCost) const;
	void PrepareSpell(TSubclassOf<ASpellCast> SpellCastClass, float ManaCost);

	void CastPreparedSpell(AWizardCharacter* WizardCharacter);

	static float GetRuneSequenceManaCost(TArray<URuneCast*>& RuneSequence);
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category= "Spells")
	TSoftObjectPtr<class UDataTable> SpellBookDataTable;

private:
	// Spell oriented variables
	TTuple<TWeakObjectPtr<ASpellCast>, float> PreparedSpell;
	TArray<FHitResult> SpellTargets;
	
	// Obtained Runes
	TArray<URuneCast*> Runes;

	// Runes casted
	TArray<URuneCast*> CastedRunes;
	FTimerHandle ClearCastedRunesTimerHandle;

	TMap<TArray<URuneCast*>, TSubclassOf<ASpellCast>> SpellBook;
	
	bool bIsCastingSpell = false;
};
