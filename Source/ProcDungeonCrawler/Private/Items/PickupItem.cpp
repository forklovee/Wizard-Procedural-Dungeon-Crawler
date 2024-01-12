#include "Items/PickupItem.h"


APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ItemMesh"));
	RootComponent = ItemMesh;
}

void APickupItem::SetSimulatePhysics(const bool bNewSimulatePhysics)
{
	ItemMesh->SetSimulatePhysics(bNewSimulatePhysics);
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}
