#include "Props/Door.h"

#include "Characters/Human/Human.h"
#include "Items/KeyItem.h"


ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = false;

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	RootComponent = FrameMesh;
	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	DoorRoot->SetupAttachment(FrameMesh);
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorRoot);
}

void ADoor::Interact(AHuman* HumanCharacter)
{
	if (bIsLocked)
	{
		return;
	}
	bIsLocked = false;
	
	SetOpen(!bIsOpen);
	
	Super::Interact(HumanCharacter);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	SetOpen(bIsOpen);
}

void ADoor::SetOpen(bool bNewIsOpen)
{
	bIsOpen = bNewIsOpen;

	if (bIsOpen)
	{
		
	}
}
