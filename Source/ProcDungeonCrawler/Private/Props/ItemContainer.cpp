// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/ItemContainer.h"

#include "Items/PickupItem.h"

AItemContainer::AItemContainer()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AItemContainer::BeginPlay()
{
	Super::BeginPlay();
}

void AItemContainer::OnDestroyed_Implementation()
{
	TArray<TSubclassOf<APickupItem>> ItemClassesToSpawn;
	ItemsInside.GetKeys(ItemClassesToSpawn);

	for (TSubclassOf<APickupItem>& ItemClass: ItemClassesToSpawn)
	{
		for (int32 i = 0; i < ItemsInside[ItemClass]; i++)
		{
			FVector SpawnLocation = GetActorLocation();
			const APickupItem* NewItem = GetWorld()->SpawnActor<APickupItem>(ItemClass, SpawnLocation, FRotator::ZeroRotator);
			NewItem->ItemMesh->AddImpulse(FVector(
				FMath::RandRange(-1.f, 1.f),
				FMath::RandRange(-1.f, 1.f),
				FMath::RandRange(-1.f, 1.f)) * 100.f
				);
		}
	}
	
	Super::OnDestroyed_Implementation();
}


