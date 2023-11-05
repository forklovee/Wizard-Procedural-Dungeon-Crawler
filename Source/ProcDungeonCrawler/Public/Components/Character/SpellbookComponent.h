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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCastedRunesCleared);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellCasted, URuneCast*, RuneCast);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API USpellbookComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FOnRuneAdded OnRuneAdded;
	FOnRuneCasted OnRuneCasted;
	FOnCastedRunesCleared OnCastedRunesCleared;
	
	FOnSpellCasted OnSpellCasted;
	
	USpellbookComponent();

	// Runes
	bool AddRune(URuneCast* NewRuneCast);

	URuneCast* GetRuneOfIdx(int Idx);
	void CastRune(URuneCast* RuneCast);
	UFUNCTION()
	void CastRuneOfIdx(int Idx);
	bool IsRuneSequenceValid(TArray<URuneCast*>& SpellRunes, TSubclassOf<ASpellCast>& OutSpellCastClass);

	void ClearCastedRunes();
	
	// Spells
	bool IsSpellPrepared() const;
	ASpellCast* GetPreparedSpell() const;
	void PrepareSpell(TSubclassOf<ASpellCast> SpellCastClass);

	void CastPreparedSpell(AWizardCharacter* WizardCharacter);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category= "Spells")
	TSoftObjectPtr<class UDataTable> SpellBookDataTable;

private:
	// Spell oriented variables
	TWeakObjectPtr<ASpellCast> PreparedSpell;
	TArray<FHitResult> SpellTargets;
	
	// Obtained Runes
	TArray<URuneCast*> Runes;

	// Runes casted
	TArray<URuneCast*> CastedRunes;
	FTimerHandle ClearCastedRunesTimerHandle;

	TMap<TArray<URuneCast*>, TSubclassOf<ASpellCast>> SpellBook;
	
	bool bIsCastingSpell = false;
};
