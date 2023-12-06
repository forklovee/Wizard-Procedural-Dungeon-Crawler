// Fill out your copyright notice in the Description page of Project Settings.

#include "Props/Interactive/LockedDoor.h"

#include "Characters/Player/WizardPlayer.h"
#include "Components/BoxComponent.h"
#include "Components/Character/BagComponent.h"
#include "Items/Locks/DoorKey.h"

ALockedDoor::ALockedDoor(): Super()
{
	LockUnlockTrigger = CreateDefaultSubobject<UBoxComponent>(FName("LockUnlockTrigger"));
	LockUnlockTrigger->SetupAttachment(DoorMesh);

	LockUnlockTrigger->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

bool ALockedDoor::Interact_Implementation(APawn* Pawn)
{
	bIsLocked = !CanPawnUnlock(Pawn);
	if (!bIsLocked)
	{
		return Super::Interact_Implementation(Pawn);
	}
	return false;
}

void ALockedDoor::BeginPlay()
{
	Super::BeginPlay();
	
	LockUnlockTrigger->ComponentTags.Add(FName("Lock"));
	LockUnlockTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALockedDoor::OnLockUnlockTriggerOverlapBegin);
}

bool ALockedDoor::CanPawnUnlock(APawn* Pawn) const
{
	if (!bIsLocked) return true;
	
	if (const AWizardPlayer* Player = Cast<AWizardPlayer>(Pawn))
	{
		if (Player->Bag->GetItemByClass(RequiredKeyClass) != nullptr)
		{
			Player->Bag->RemoveItem(RequiredKeyClass);
			return true;
		}
	}
	return false;
}

void ALockedDoor::OnLockUnlockTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || !bIsLocked) return;
	if (!OtherActor->Tags.Contains("Key")) return;

	if (ADoorKey* DoorKey = Cast<ADoorKey>(OtherActor))
	{
		LockUnlockTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &ALockedDoor::OnLockUnlockTriggerOverlapBegin);
		bIsLocked = false;

		DoorKey->Destroy();
		
		UE_LOG(LogTemp, Display, TEXT("Door %s, overlapped by key %s"), *GetName(), *OtherActor->GetName());
	}
}
