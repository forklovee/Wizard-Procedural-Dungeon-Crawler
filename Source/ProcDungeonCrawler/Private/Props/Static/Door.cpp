// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/Static/Door.h"


ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	SetOpen(bInitAsOpen);
}

void ADoor::SetOpen(bool bNewIsOpen)
{
	bIsOpen = bNewIsOpen;
	if (OnDoorStateChanged.IsBound())
	{
		OnDoorStateChanged.Broadcast(bIsOpen);
	}
}
