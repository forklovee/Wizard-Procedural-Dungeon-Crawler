// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/BagActor.h"

#include "Components/WidgetComponent.h"
#include "Items/PickupItem.h"

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
		SpawnActors();
	}
}

void ABagActor::SetPawnItems(TMap<TSubclassOf<APickupItem>, int32>& Items)
{
	PawnItems = &Items;
}

void ABagActor::BeginPlay()
{
	Super::BeginPlay();

	SpawnedItems.SetNum(ViewSlots);
}

FVector ABagActor::GetSlotLocation(int SlotIdx) const
{
	const FVector SlotsOrigin = FVector::LeftVector * SlotOffset * (ViewSlots - 1) / 2.f;
	return RootComponent->GetRelativeLocation() + SlotsOrigin + (FVector::LeftVector * SlotOffset * SlotIdx);
}

void ABagActor::ChangeSlotsPage(int PageOffset)
{
	PageOffset = -1; // Right
	
	TArray<TSubclassOf<APickupItem>> ItemClasses;
	PawnItems->GetKeys(ItemClasses);
	
	LastViewStartIdx = ViewStartIdx;
	ViewStartIdx += PageOffset;
	ViewStartIdx = FMath::Clamp(ViewStartIdx, 0, PawnItems->Num() - ViewSlots);
	
	for (int ActorIdx = 0; ActorIdx < SpawnedItems.Num(); ActorIdx++)
	{
		AActor* ItemActor = SpawnedItems[ActorIdx];
		if (ItemActor == nullptr)
		{
			const int NewItemIdx = ViewStartIdx + ActorIdx - PageOffset;
			if (NewItemIdx < 0 || NewItemIdx > ItemClasses.Num())
			{
				continue;
			}
			const TSubclassOf<APickupItem> NewItemClass = ItemClasses[NewItemIdx];
			const FVector ItemLocation = GetSlotLocation(ActorIdx);
			const FRotator ItemRotation = FRotator::ZeroRotator;
			SpawnedItems[ActorIdx] = GetWorld()->SpawnActor(NewItemClass->GetClass());

			if (SpawnedItems[ActorIdx] != nullptr)
			{
				SpawnedItems[ActorIdx]->SetActorLocation(ItemLocation);
			}
		}
		
		const int NewActorIdx = ActorIdx + PageOffset;
		if (NewActorIdx < 0 || NewActorIdx >= ViewSlots)
		{
			ItemActor->Destroy();
			SpawnedItems[ActorIdx] = nullptr;
			continue;
		}

		SpawnedItems[NewActorIdx] = SpawnedItems[ActorIdx];
	}
}

void ABagActor::SpawnActors()
{
	TArray<TSubclassOf<APickupItem>> ItemClasses;
	PawnItems->GetKeys(ItemClasses);
	for (int idx = ViewStartIdx; idx < ViewSlots; idx++)
	{
		if (idx >= ItemClasses.Num() || idx < 0)
		{
			break;
		}
		const TSubclassOf<APickupItem>& ItemClass = ItemClasses[idx];
		
	}
}

