// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/BagComponent.h"

#include "Items/PickupItem.h"
#include "Characters/Player/BagActor.h"

UBagComponent::UBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UBagComponent::IsOpen() const
{
	return BagActor.IsValid();
}

void UBagComponent::SetBagActorAttached(const bool bIsAttached)
{
	if (!BagActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to set Bag Actor attachment (actor is null)!"));
		return;
	}
	
	if (bIsAttached)
	{
		BagActor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
	}
	else
	{
		BagActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
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
		SetBagActorAttached(true);
	}

	if (OnBagStateChanged.IsBound())
	{
		OnBagStateChanged.Broadcast(IsOpen());
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

	if (OnBagContentsUpdated.IsBound())
	{
		OnBagContentsUpdated.Broadcast();
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

	if (OnBagContentsUpdated.IsBound())
	{
		OnBagContentsUpdated.Broadcast();
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
