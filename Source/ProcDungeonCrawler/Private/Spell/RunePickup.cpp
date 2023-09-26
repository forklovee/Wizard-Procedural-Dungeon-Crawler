// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/RunePickup.h"

#include "WizardCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Spell/RuneCast.h"

// Sets default values
ARunePickup::ARunePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("RuneCollision"));
	PickupCollision->SetCollisionProfileName(FName("OverlapOnlyPawn"));
	RootComponent = PickupCollision;

	RuneStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("RuneStaticMesh"));
	RuneStaticMesh->SetupAttachment(RootComponent);
	RuneStaticMesh->SetEnableGravity(false);
	RuneStaticMesh->SetCollisionProfileName(FName("NoCollision"));

	RuneSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("RuneSkeletalMesh"));
	RuneSkeletalMesh->SetupAttachment(RootComponent);
	RuneSkeletalMesh->SetEnableGravity(false);
	RuneSkeletalMesh->SetCollisionProfileName(FName("NoCollision"));
}

AActor* ARunePickup::Pickup_Implementation(APawn* Pawn)
{
	IPropPickupInterface::Pickup_Implementation(Pawn);

	PickupCollision->SetCollisionProfileName(FName("NoCollision"));
	RuneStaticMesh->SetVisibility(false);
	RuneSkeletalMesh->SetVisibility(false);
	
	return this;
}

FName ARunePickup::GetItemName_Implementation()
{
	FName RuneName = "Rune";
	if (RuneCast.IsValid())
	{
		RuneName = RuneCast->RuneName;
	}
	return RuneName;
}

FName ARunePickup::GetInteractionName_Implementation()
{
	return FName("Pick up");
}

// Called when the game starts or when spawned
void ARunePickup::BeginPlay()
{
	Super::BeginPlay();
	
}

