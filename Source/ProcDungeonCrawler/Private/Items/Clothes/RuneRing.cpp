// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Clothes/RuneRing.h"

ARuneRing::ARuneRing()
{
	PrimaryActorTick.bCanEverTick = false;
}

TSoftObjectPtr<URune>& ARuneRing::GetRuneCast()
{
	return RuneCast;
}