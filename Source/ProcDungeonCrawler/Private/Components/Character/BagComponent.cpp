// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/BagComponent.h"

#include "Items/PickupItem.h"
#include "Player/BagActor.h"

UBagComponent::UBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UBagComponent::IsOpen() const
{
	return BagActor.IsValid();
}

void UBagComponent::ToggleBag()
{
	if (BagActor.IsValid())
	{
		BagActor.Get()->Destroy();
		BagActor = nullptr;
	}
	else
	{
		const FVector BagLocation = GetComponentLocation();
		FRotator BagRotation = GetComponentRotation();
		BagRotation.Yaw += 180.0f;
		
		BagActor = Cast<ABagActor>(GetWorld()->SpawnActor(BagActorClass, &BagLocation, &BagRotation));
		BagActor->SetPawnItems(Items);

		BagActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
	}
}

void UBagComponent::AddItem(TSubclassOf<APickupItem> ItemClass, int32 Amount)
{
	if (!HasItemClass(ItemClass))
	{
		Items.Add(ItemClass, Amount);
	}
	else
	{
		Items[ItemClass] += Amount;
	}

	if (OnBagUpdated.IsBound())
	{
		OnBagUpdated.Broadcast();
	}
}

void UBagComponent::RemoveItem(TSubclassOf<APickupItem> ItemClass, int32 Amount)
{
	if (!HasItemClass(ItemClass)) return;

	const int32 ItemAmount = GetItemAmount(ItemClass);
	if (ItemAmount - Amount <= 0)
	{
		Items.Remove(ItemClass);
	}
	else
	{
		Items[ItemClass] -= Amount;
	}

	if (OnBagUpdated.IsBound())
	{
		OnBagUpdated.Broadcast();
	}
}

bool UBagComponent::HasItemClass(TSubclassOf<APickupItem> ItemClass) const
{
	return Items.Contains(ItemClass);
}

int32 UBagComponent::GetItemAmount(TSubclassOf<APickupItem> ItemClass) const
{
	if (HasItemClass(ItemClass))
	{
		return Items[ItemClass];
	}
	return 0;
}
