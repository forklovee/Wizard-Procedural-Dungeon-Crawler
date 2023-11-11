// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Locks/LockKey.h"

#include "Components/SphereComponent.h"

ALockKey::ALockKey()
{
	UnlockTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("UnlockTrigger"));
	UnlockTrigger->SetupAttachment(RootComponent);
}
