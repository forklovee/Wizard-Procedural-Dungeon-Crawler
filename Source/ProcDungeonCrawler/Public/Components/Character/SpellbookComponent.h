// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellbookComponent.generated.h"

class AWizardCharacter;
class URuneCast;
struct FInputActionValue;
class ASpellCast;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRuneAdded, URuneCast*, RuneCast);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API USpellbookComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FOnRuneAdded OnRuneAdded;
	
	USpellbookComponent();

	// Runes
	bool AddRune(TSoftObjectPtr<URuneCast> NewRuneCast);

	void CastRune(const FInputActionValue& Value);
	bool IsRuneSequenceValid(TArray<TSoftObjectPtr<URuneCast>> SpellRunes, TSubclassOf<ASpellCast>& OutSpellCastClass);

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
	TArray<TSoftObjectPtr<URuneCast>> Runes;

	// Runes casted
	TArray<TSoftObjectPtr<URuneCast>> CastedRunes;
	FTimerHandle ClearCastedRunesTimerHandle;

	// Spell data RuneChain id == SpellCastClass id
	// Correct SpellCast for RuneChain is get from IsRuneSequenceValid function
	TArray<TArray<TSoftObjectPtr<URuneCast>>> RuneChains;
	TArray<TSubclassOf<ASpellCast>> SpellCastClasses;
	
	bool bIsCastingSpell = false;
	
};
