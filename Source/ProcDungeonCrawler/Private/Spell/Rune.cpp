// Fill out your copyright notice in the Description page of Project Settings.

#include "Spell/Rune.h"

void URune::CastRune(AHuman* Human)
{
	if (OnRuneCasted.IsBound())
	{
		OnRuneCasted.Broadcast(Human, this);
	}
}
