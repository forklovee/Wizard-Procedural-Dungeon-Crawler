#include "World/DungeonGenerator/Rooms/RoomPCGGlobalVolume.h"

#include "PCGComponent.h"

ARoomPCGGlobalVolume::ARoomPCGGlobalVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	
	PCGComponent = CreateDefaultSubobject<UPCGComponent>(FName("PCGComponent"));
}

void ARoomPCGGlobalVolume::BeginPlay()
{
	Super::BeginPlay();
}

