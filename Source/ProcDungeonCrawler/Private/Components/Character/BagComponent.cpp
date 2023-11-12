// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/BagComponent.h"

#include "Components/WidgetComponent.h"
#include "Items/PickupItem.h"
#include "Characters/Player/BagActor.h"

UBagComponent::UBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBagComponent::OnPlayerCursorOnWidgetHoverChanged(UWidgetComponent* WidgetComponent,
	UWidgetComponent* PreviousWidgetComponent)
{
	const bool NewPlayerCursorInBounds = WidgetComponent != nullptr && PreviousWidgetComponent == nullptr;
	if (OnPlayerCursorHoverChanged.IsBound() && NewPlayerCursorInBounds != bPlayerCursorInBounds)
	{
		OnPlayerCursorHoverChanged.Broadcast(NewPlayerCursorInBounds);
	}
	bPlayerCursorInBounds = NewPlayerCursorInBounds;
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
		OnPlayerLeftRightInput.RemoveDynamic(BagActor.Get(), &ABagActor::ChangeSlotsPage);
		OnPlayerCursorHoverChanged.RemoveDynamic(BagActor.Get(), &ABagActor::OnPlayerCursorHoverChanged);
		BagActor->ClearView();
		BagActor.Get()->Destroy();
		BagActor = nullptr;
		bPlayerCursorInBounds = false;
	}
	else
	{
		if (BagActorClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Bag Actor Class is null!"));
			return;
		}
		
		const FVector BagLocation = GetComponentLocation();
		FRotator BagRotation = GetComponentRotation();
		BagRotation.Yaw += 180.0f;
		
		BagActor = Cast<ABagActor>(GetWorld()->SpawnActor(BagActorClass, &BagLocation, &BagRotation));
		BagActor->SetPawnItems(Items);
		
		OnPlayerLeftRightInput.AddDynamic(BagActor.Get(), &ABagActor::ChangeSlotsPage);
		OnPlayerCursorHoverChanged.AddDynamic(BagActor.Get(), &ABagActor::OnPlayerCursorHoverChanged);
		
		SetBagActorAttached(true);
		bPlayerCursorInBounds = false;
	}

	if (OnBagStateChanged.IsBound())
	{
		OnBagStateChanged.Broadcast(IsOpen());
	}
}

void UBagComponent::SetupBagActor()
{
	if (BagActor.IsValid())
	{
		BagActor->SetupView();
	}
}

void UBagComponent::OnLeftRightInputAction(int Direction)
{
	if (OnPlayerLeftRightInput.IsBound())
	{
		OnPlayerLeftRightInput.Broadcast(Direction);
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
