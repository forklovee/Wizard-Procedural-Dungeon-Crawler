// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\Public\Spell\RuneCast.h"

void URuneCast::CastRune(AWizardCharacter* WizardCharacter)
{
	if (OnRuneCasted.IsBound())
	{
		OnRuneCasted.Broadcast(WizardCharacter, this);
	}
}
