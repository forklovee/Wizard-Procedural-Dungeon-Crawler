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
	const FVector RightVector = GetActorForwardVector().RotateAngleAxis(90.f, FVector::UpVector);
	const FVector SlotsOrigin = GetActorForwardVector()*-25.f + FVector::UpVector*25.f + (RightVector * 25.f * (ViewSlots - 1) / 2.f);
	return GetActorLocation() + SlotsOrigin + (RightVector * 25.f * SlotIdx);
}

void ABagActor::SetupView()
{
	ViewStartIdx = 0;
	TArray<TSubclassOf<APickupItem>> ItemClasses;
	PawnItems->GetKeys(ItemClasses);
	for (int idx = ViewStartIdx; idx < ViewSlots; idx++)
	{
		if (idx >= ItemClasses.Num() || idx < 0)
		{
			break;
		}
		const TSubclassOf<APickupItem>& ItemClass = ItemClasses[idx];
		const FVector ItemLocation = GetSlotLocation(idx);
		const FRotator ItemRotation = FRotator::ZeroRotator;
		
		SpawnedItems[idx] = Cast<APickupItem>(
			GetWorld()->SpawnActor(ItemClass->GetDefaultObject()->GetClass(), &ItemLocation, &ItemRotation)
			);
		if (SpawnedItems[idx] == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn item: %s"), *ItemClass->GetName());
		}
		else
		{
			SpawnedItems[idx]->SetSimulatePhysics(false);
		}
	}
}

void ABagActor::ClearView()
{
	for (int Idx = 0; Idx < SpawnedItems.Num(); Idx++)
	{
		if (SpawnedItems[Idx] != nullptr)
		{
			SpawnedItems[Idx]->Destroy();
			SpawnedItems[Idx] = nullptr;
		}
	}
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
		APickupItem* ItemActor = SpawnedItems[ActorIdx];
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

			SpawnedItems[ActorIdx] = Cast<APickupItem>(
			GetWorld()->SpawnActor(NewItemClass->GetDefaultObject()->GetClass(), &ItemLocation, &ItemRotation)
			);
			if (SpawnedItems[ActorIdx] == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn item: %s"), *NewItemClass->GetName());
			}
			else
			{
				SpawnedItems[ActorIdx]->SetSimulatePhysics(false);
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
