// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BagActor.h"

#include "Components/WidgetComponent.h"

ABagActor::ABagActor()
{
	PrimaryActorTick.bCanEverTick = false;

	BagMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BagMesh"));
	RootComponent = BagMesh;

	BagWidget = CreateDefaultSubobject<UWidgetComponent>(FName("BagWidget"));
	BagWidget->SetupAttachment(BagMesh);
	BagWidget->SetManuallyRedraw(true);
}

void ABagActor::ToggleBag()
{
	bIsOpen = !bIsOpen;
	if (bIsOpen)
	{
		BagWidget->UpdateWidget();
	}
	
}

