// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BagActor.h"

#include "Components/WidgetComponent.h"

ABagActor::ABagActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));;

	BagMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BagMesh"));
	BagMesh->SetupAttachment(RootComponent);
	
	SlotsRootComponent = CreateDefaultSubobject<USceneComponent>(FName("SlotsRootComponent"));
	SlotsRootComponent->SetupAttachment(BagMesh);
	
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

void ABagActor::SetPawnItems(TMap<TSubclassOf<APickupItem>, int32>& Items)
{
	PawnItems = &Items;
}

FVector ABagActor::GetSlotLocation(int SlotIdx) const
{
	const FVector SlotsOrigin = FVector::LeftVector * SlotOffset * (ViewSlots - 1) / 2.f;
	return RootComponent->GetRelativeLocation() + SlotsOrigin + (FVector::LeftVector * SlotOffset * SlotIdx);
}

void ABagActor::ChangeSlotsPage(int PageOffset)
{
	
}

void ABagActor::SpawnActors()
{
	
}

