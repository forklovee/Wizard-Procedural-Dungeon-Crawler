// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/BagComponent.h"

#include "Components/WidgetComponent.h"
#include "Items/PickupItem.h"
#include "Characters/Player/BagActor.h"
#include "Characters/Player/WizardPlayer.h"
#include "Components/Character/PlayerInteractionRaycast.h"

UBagComponent::UBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBagComponent::OnPlayerCursorOnWidgetHoverChanged(UWidgetComponent* WidgetComponent,
	UWidgetComponent* PreviousWidgetComponent)
{
	const bool NewPlayerCursorInBounds = WidgetComponent != nullptr && PreviousWidgetComponent == nullptr;
	if (OnPlayerCursorHoverChanged.IsBound() && NewPlayerCursorInBounds != bPlayerCursorInBounds && BagOwner.IsValid())
	{
		OnPlayerCursorHoverChanged.Broadcast(NewPlayerCursorInBounds, BagOwner->PlayerInteraction->GetGrabbedActor());
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
		if (BagActor.IsValid())
		{
			BagActor->SetPawnItems(Items);

			BagActor->OnGrabbedActorPositionOverrideRequest.AddDynamic(BagOwner->PlayerInteraction, &UPlayerInteractionRaycast::SetGrabbedActorPositionOverride);
			BagActor->OnGrabbedActorPositionOverrideClearRequest.AddDynamic(BagOwner->PlayerInteraction, &UPlayerInteractionRaycast::ClearGrabbedActorPositionOverride);
			BagOwner->PlayerInteraction->OnPropGrabbed.AddDynamic(BagActor.Get(), &ABagActor::BeginItemGrab);
			BagOwner->PlayerInteraction->OnPropReleased.AddDynamic(BagActor.Get(), &ABagActor::CommitGrabbedItemAction);
			
			// Input events binding
			OnPlayerLeftRightInput.AddDynamic(BagActor.Get(), &ABagActor::ChangeSlotsPage);
			OnPlayerCursorHoverChanged.AddDynamic(BagActor.Get(), &ABagActor::OnPlayerCursorHoverChanged);

			// Item events binding
			BagActor->OnAddItemRequest.AddDynamic(this, &UBagComponent::OnAddItemRequest);
			BagActor->OnRemoveItemRequest.AddDynamic(this, &UBagComponent::OnRemoveItemRequest);
			OnNewItemAdded.AddDynamic(BagActor.Get(), &ABagActor::OnNewItemAdded);
			OnItemRemoved.AddDynamic(BagActor.Get(), &ABagActor::OnItemRemoved);
		}
		
		
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
	
	if (OnNewItemAdded.IsBound())
	{
		OnNewItemAdded.Broadcast(ItemClass, Items[ItemClass]);
	}
	
	if (OnBagContentsUpdated.IsBound()) //TODO: if its really necessary???
	{
		OnBagContentsUpdated.Broadcast();
	}
}

void UBagComponent::OnAddItemRequest(TSubclassOf<APickupItem> ItemClass)
{
	AddItem(ItemClass);
}

void UBagComponent::RemoveItem(TSubclassOf<APickupItem> ItemClass, int32 Amount)
{
	if (!HasItemClass(ItemClass)) return;

	int32 ItemAmount = GetItemAmount(ItemClass);
	if (ItemAmount - Amount <= 0)
	{
		Items.Remove(ItemClass);
		ItemAmount = 0;
	}
	else
	{
		Items[ItemClass] -= Amount;
		ItemAmount = Items[ItemClass];
	}
	
	if (OnItemRemoved.IsBound())
	{
		OnItemRemoved.Broadcast(ItemClass, ItemAmount);
	}
	
	if (OnBagContentsUpdated.IsBound()) //TODO: if its really necessary???
	{
		OnBagContentsUpdated.Broadcast();
	}
}

void UBagComponent::OnRemoveItemRequest(TSubclassOf<APickupItem> ItemClass)
{
	RemoveItem(ItemClass);
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

void UBagComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AWizardPlayer* WizardPawn = Cast<AWizardPlayer>(GetOwner()))
	{
		BagOwner = WizardPawn;
	}
}
