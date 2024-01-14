// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffect/StatusEffect.h"

void AStatusEffect::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		EffectApplyTimerHandle,
		this,
		&AStatusEffect::ApplyEffect,
		TickRate,
		true,
		0.f);
}

void AStatusEffect::ApplyEffect()
{
	if (OnTimedApplyEffect.IsBound())
	{
		OnTimedApplyEffect.Broadcast(this);
	}
}
