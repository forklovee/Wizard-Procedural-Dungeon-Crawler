// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/BagActor.h"

#include "Components/WidgetComponent.h"
#include "Items/PickupItem.h"
#include "UI/Bag/BagItemTile.h"
#include "UI/Bag/BagUI.h"

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

void ABagActor::OnPlayerCursorHoverChanged(bool bIsHovered)
{
	if (!bIsHovered && GrabbedItemTile != nullptr)
	{
		const TSubclassOf<APickupItem> ItemClass = GrabbedItemTile->GetPickupItemClass();
		if (int* ItemAmountPtr = PawnItems->Find(ItemClass))
		{
			*ItemAmountPtr -= 1;
			if (*ItemAmountPtr <= 0)
			{
				PawnItems->Remove(ItemClass);
				GrabbedItemTile->SetPickupItemClass(nullptr);
			}
			else
			{
				SpawnItemActor(GrabbedItemTile.Get());
			}
			return;
		}
		GrabbedItemTile->SetPickupItemClass(nullptr);
	}
}

void ABagActor::SetPawnItems(TMap<TSubclassOf<APickupItem>, int32>& Items)
{
	PawnItems = &Items;
}

void ABagActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABagActor::SetGrabbedItemTile(UBagItemTile* Item, bool bIsGrabbed)
{
	GrabbedItemTile = bIsGrabbed ? Item : nullptr;
}

FVector ABagActor::GetSlotLocation(int SlotIdx) const
{
	const FVector RightVector = GetActorForwardVector().RotateAngleAxis(90.f, FVector::UpVector);
	const FVector SlotsOrigin = GetActorForwardVector()*-25.f + FVector::UpVector*25.f + (RightVector * 25.f * (ViewSlots - 1) / 2.f);
	return GetActorLocation() + SlotsOrigin - (RightVector * 25.f * SlotIdx);
}

void ABagActor::SpawnItemActor(UBagItemTile* ItemTile)
{
	const int32 ItemIndex = ItemTile->GetIndex();
	const TSubclassOf<APickupItem> ItemClass = *ItemTile->GetPickupItemClass();
	const FVector ItemLocation = GetSlotLocation(ItemIndex);
	const FRotator ItemRotation = FRotator::ZeroRotator;

	APickupItem* PickupItemActor = Cast<APickupItem>(
			GetWorld()->SpawnActor(ItemClass->GetDefaultObject()->GetClass(), &ItemLocation, &ItemRotation)
			);
	PickupItemActor->SetSimulatePhysics(false);

	if (!ItemTile->bItemActorGrabChangeEventConnected)
	{
		ItemTile->OnItemActorGrabbedChanged.AddDynamic(this, &ABagActor::SetGrabbedItemTile);
		ItemTile->bItemActorGrabChangeEventConnected = true;
	}
	
	ItemTile->SetPickupItemActor( PickupItemActor );
}

void ABagActor::DestroyItemActor(UBagItemTile* ItemTile)
{
	if (APickupItem* PickupItemActor = ItemTile->GetPickupItemActor())
	{
		ItemTile->SetPickupItemActor(nullptr);
		PickupItemActor->Destroy();
	}
}

void ABagActor::SetupView()
{
	TArray<TSubclassOf<APickupItem>> ItemClasses;
	PawnItems->GetKeys(ItemClasses);
	
	if (UBagUI* BagUI = Cast<UBagUI>(BagWidget->GetWidget()))
	{
		BagUI->SetupBagUI(this, ItemClasses);

		BagUI->ChangePage(0);
	}
}

void ABagActor::ClearView()
{
	if (UBagUI* BagUI = Cast<UBagUI>(BagWidget->GetWidget()))
	{
		for (APickupItem* PickupItem : BagUI->GetAllSpawnedActors())
		{
			PickupItem->Destroy();
		}
	}
}

void ABagActor::ChangeSlotsPage(int PageOffset)
{
	
}
