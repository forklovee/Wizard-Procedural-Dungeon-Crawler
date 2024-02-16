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
	DoorMesh->ComponentTags.Add(FName("InteractiveObject"));
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	SetOpen(bIsOpen, nullptr);
}

bool ADoor::Interact(AHuman* HumanCharacter)
{
	if (RequiredKeyClass != nullptr)
	{
		int OutAmount = 0;
		if (HumanCharacter->Inventory->HasItemOfClass(RequiredKeyClass, OutAmount))
		{
			HumanCharacter->Inventory->RemoveItemByClass(RequiredKeyClass, 1);
			RequiredKeyClass = nullptr;
			InteractionSuccess(HumanCharacter);
			return Super::Interact(HumanCharacter);
		}
		InteractionFailed(HumanCharacter);
		return false;
	}
	InteractionSuccess(HumanCharacter);
	return Super::Interact(HumanCharacter);
}

void ADoor::InteractionSuccess(AHuman* HumanCharacter)
{
	Super::InteractionSuccess(HumanCharacter);
	SetOpen(!bIsOpen, HumanCharacter);
}

void ADoor::InteractionFailed(AHuman* HumanCharacter)
{
	Super::InteractionFailed(HumanCharacter);
}

void ADoor::SetOpen(bool bNewIsOpen, AHuman* HumanCharacter)
{
	bIsOpen = bNewIsOpen;

	if (bIsOpen)
	{
		if (OnDoorOpened.IsBound())
		{
			OnDoorOpened.Broadcast(HumanCharacter);
		}
	}
	else
	{
		if (OnDoorClosed.IsBound())
		{
			OnDoorClosed.Broadcast(HumanCharacter);
		}
	}
}
