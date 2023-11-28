

#include "World/DungeonGenerator/Rooms/RoomDoorMarker.h"

UDoorMarkerVolume::UDoorMarkerVolume()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDoorMarkerVolume::BeginPlay()
{
	Super::BeginPlay();

	ComponentTags.Add(FName("RoomDoorMarker"));
}

