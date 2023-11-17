// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Locks/DoorKey.h"
#include "Components/SphereComponent.h"

ADoorKey::ADoorKey(): Super()
{
	UnlockTrigger = CreateDefaultSubobject<USphereComponent>(FName("UnlockTrigger"));
	UnlockTrigger->SetupAttachment(RootComponent);
}

void ADoorKey::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Key"));
	
	UnlockTrigger->SetGenerateOverlapEvents(true);
	UnlockTrigger->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void ADoorKey::Grab_Implementation()
{
	Super::Grab_Implementation();

	if (!bIsUnlockTriggerConnected)
	{
		UnlockTrigger->OnComponentBeginOverlap.AddDynamic(this, &ADoorKey::OnUnlockTriggerOverlapBegin);
		bIsUnlockTriggerConnected = true;
	}
}

void ADoorKey::Release_Implementation()
{
	Super::Release_Implementation();

	if (bIsUnlockTriggerConnected)
	{
		UnlockTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &ADoorKey::OnUnlockTriggerOverlapBegin);
		bIsUnlockTriggerConnected = false;
	}
}

void ADoorKey::OnUnlockTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherComp == nullptr || !TargetDoor.IsValid()) return;
	if (!OtherActor->Tags.Contains("Door")) return;
	if (OtherActor != Cast<AActor>(TargetDoor.Get()) &&
		Cast<UShapeComponent>(OtherComp) == nullptr && OtherComp->ComponentTags.Contains("Lock")) return;
	
	UE_LOG(LogTemp, Display, TEXT("Key Overlaps with %s"), *OtherActor->GetName())
}
