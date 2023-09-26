// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/BagComponent.h"

UBagComponent::UBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBagComponent::AddItem(TSubclassOf<AActor> ItemClass, int32 Amount)
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
		OnBagUpdated.Broadcast(GetItemClasses());
	}
}

void UBagComponent::RemoveItem(TSubclassOf<AActor> ItemClass, int32 Amount)
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
		OnBagUpdated.Broadcast(GetItemClasses());
	}
}

bool UBagComponent::HasItemClass(TSubclassOf<AActor> ItemClass) const
{
	return Items.Contains(ItemClass);
}

int32 UBagComponent::GetItemAmount(TSubclassOf<AActor> ItemClass) const
{
	if (HasItemClass(ItemClass))
	{
		return Items[ItemClass];
	}
	return 0;
}

TSet<TSubclassOf<AActor>> UBagComponent::GetItemClasses() const
{
	TSet<TSubclassOf<AActor>> ItemClasses;
	Items.GetKeys(ItemClasses);
	return ItemClasses;
}
