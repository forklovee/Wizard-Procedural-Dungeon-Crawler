// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.generated.h"

UENUM(BlueprintType)
enum class EEffectType: uint8
{
	Hurt,
	Heal,
	Fire,
	Water,
	Stun,
	Shock,
	Freeze,
	SlowDown,
	SpeedUp
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimedApplyEffect, AStatusEffect*, StatusEffect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectEnd, AStatusEffect*, StatusEffect);

UCLASS()
class AStatusEffect : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnTimedApplyEffect OnTimedApplyEffect;
	UPROPERTY(BlueprintAssignable)
	FOnEffectEnd OnEffectEnd;

protected:
	virtual void BeginPlay() override;

private:
	void ApplyEffect();
	
public:
	UPROPERTY(EditAnywhere)
	EEffectType EffectType = EEffectType::Hurt;

	UPROPERTY(EditAnywhere)
	float Strength = 1.0;

	UPROPERTY(EditAnywhere)
	float Duration = 4.0;

	UPROPERTY(EditAnywhere)
	float TickRate = 2.0;

	TWeakObjectPtr<AActor> Instigator = nullptr;
private:
	FTimerHandle EffectApplyTimerHandle;
};
