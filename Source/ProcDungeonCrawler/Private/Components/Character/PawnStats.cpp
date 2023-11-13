// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/PawnStats.h"

#include "Spell/SpellCast.h"
#include "StatusEffect/StatusEffect.h"

UPawnStats::UPawnStats()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPawnStats::UpdateUIStats()
{
	Health = StartingHealth;
	Mana = StartingMana;
	
	if (OnHeal.IsBound())
	{
		OnHeal.Broadcast(Health, 0.f);
	}
	if (OnManaUsage.IsBound())
	{
		OnManaUsage.Broadcast(Mana, 0.f);
	}
}

void UPawnStats::BeginPlay()
{
	Super::BeginPlay();

	Health = StartingHealth;
	Mana = StartingMana;
	
	OwnerPawn = Cast<APawn>(GetOwner());
}

float UPawnStats::GetMaxHealth() const
{
	return MaxHealth;
}

float UPawnStats::GetHealth() const
{
	return Health;
}

float UPawnStats::GetMaxMana() const
{
	return MaxMana;
}

float UPawnStats::GetMana() const
{
	return Mana;
}

bool UPawnStats::CanCastSpell(ASpellCast* SpellCast)
{
	if (SpellCast == nullptr)
	{
		return false;
	}
	
	const float SpellCost = SpellCast->ManaCost;
	if (SpellCost > Mana)
	{
		return false;
	}

	Mana = FMath::Clamp(Mana - SpellCost, 0.f, MaxMana);
	if (OnManaUsage.IsBound())
	{
		OnManaUsage.Broadcast(Mana, SpellCost);
	}
	return true;
}

void UPawnStats::Hurt(AActor* DamageCauser, float Damage, UDamageType* DamageType)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	if (OnHurt.IsBound())
	{
		OnHurt.Broadcast(DamageCauser, Damage, DamageType);
	}
}

void UPawnStats::Heal(float HealAmount)
{
	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);

	if (OnHeal.IsBound())
	{
		OnHeal.Broadcast(Health, HealAmount);
	}
}

void UPawnStats::ApplyStatusEffect(TSubclassOf<UStatusEffect> StatusEffect)
{
	UStatusEffect* NewStatusEffect = NewObject<UStatusEffect>(this, StatusEffect);
	NewStatusEffect->ApplyEffect(OwnerPawn.Get());

	if (OnStatusEffectApplied.IsBound())
	{
		OnStatusEffectApplied.Broadcast(NewStatusEffect);
	}
}