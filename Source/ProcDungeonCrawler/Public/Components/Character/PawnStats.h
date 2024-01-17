// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PawnStats.generated.h"

class ASpell;
class AStatusEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeal, float, Health, float, HealAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaUsage, float, Mana, float, ManaUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHurt, AActor*, DamageCauser, float, Damage, const UDamageType*, DamageType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewStatusEffectAdded, AStatusEffect*, StatusEffect);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UPawnStats : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnHeal OnHeal;
	UPROPERTY(BlueprintAssignable)
	FOnManaUsage OnManaUsage;
	UPROPERTY(BlueprintAssignable)
	FOnHurt OnHurt;
	UPROPERTY(BlueprintAssignable)
	FOnNewStatusEffectAdded OnNewStatusEffectAdded;
	
	UPawnStats();
	
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxMana() const;
	UFUNCTION(BlueprintCallable)
	float GetMana() const;

	UFUNCTION(BlueprintCallable)
	bool HasRequiredMana(float ManaCost) const;
	
	UFUNCTION(BlueprintCallable)
	void TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable)
	void UseMana(ASpell* SpellCast, float ManaCost);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void AddStatusEffect(TSubclassOf<AStatusEffect> StatusEffectClass);

private:
	void ApplyStatusEffect(AStatusEffect* StatusEffect);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Health")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Health")
	float StartingHealth = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Mana")
	float MaxMana = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Mana")
	float StartingMana = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	float Strength = 1.f;
	
protected:
	float Health = 0.f;
	float Mana = 0.f;
};
