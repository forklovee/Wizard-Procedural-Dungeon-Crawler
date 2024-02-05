// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/PawnStats.h"

#include "..\..\..\Public\Spell\Spell.h"
#include "StatusEffect/StatusEffect.h"

UPawnStats::UPawnStats()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPawnStats::BeginPlay()
{
	Super::BeginPlay();

	Health = StartingHealth;
	Mana = StartingMana;
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

bool UPawnStats::HasRequiredMana(float ManaCost) const
{
	return Mana >= ManaCost;
}

void UPawnStats::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	ChangeHealth(-Damage);
}

void UPawnStats::ChangeHealth(float HealthChange)
{
	Health = FMath::Clamp(Health + HealthChange, 0.f, MaxHealth);

	if (HealthChange > 0.f && OnHeal.IsBound())
	{
		OnHeal.Broadcast(Health, HealthChange);
	}
	else if (HealthChange < 0.f && OnHurt.IsBound())
	{
		OnHurt.Broadcast(Health, FMath::Abs(HealthChange));
	}
}

void UPawnStats::ChangeMana(float ManaChange)
{
	Mana = FMath::Clamp(Mana + ManaChange, 0.f, MaxMana);

	if (ManaChange > 0.f && OnManaRestore.IsBound())
	{
		OnManaRestore.Broadcast(Mana, ManaChange);
	}
	else if (ManaChange < 0.f && OnManaUsage.IsBound())
	{
		OnManaUsage.Broadcast(Mana, FMath::Abs(ManaChange));
	}
}

void UPawnStats::UseMana(ASpell* SpellCast, float ManaCost)
{
	Mana = FMath::Clamp(Mana - ManaCost, 0.f, MaxMana);
	if (OnManaUsage.IsBound())
	{
		OnManaUsage.Broadcast(Mana, ManaCost);
	}
}

void UPawnStats::AddStatusEffect(TSubclassOf<AStatusEffect> StatusEffectClass)
{
	AStatusEffect* NewStatusEffect = GetWorld()->SpawnActor<AStatusEffect>(StatusEffectClass);
	NewStatusEffect->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);

	NewStatusEffect->OnTimedApplyEffect.AddDynamic(this, &UPawnStats::ApplyStatusEffect);
	
	if (OnNewStatusEffectAdded.IsBound())
	{
		OnNewStatusEffectAdded.Broadcast(NewStatusEffect);
	}
}

void UPawnStats::ApplyStatusEffect(AStatusEffect* StatusEffect)
{
	switch (StatusEffect->EffectType)
	{
		default:
		case EEffectType::Heal:
		{
			ChangeHealth(StatusEffect->Strength);
			break;
		}
			
		case EEffectType::Hurt:
		{
			TakeDamage(GetOwner(), StatusEffect->Strength, nullptr, nullptr, StatusEffect);
		}
		
		case EEffectType::Fire:
		{
			break;
		}
		case EEffectType::Water:
		{
			break;
		}
		case EEffectType::Stun:
		{
			break;
		}
		case EEffectType::Shock:
		{
			break;
		}
		case EEffectType::Freeze:
		{
			break;
		}
		case EEffectType::SlowDown:
		{
			break;
		}
		case EEffectType::SpeedUp:
		{
			break;
		}
	}
}
