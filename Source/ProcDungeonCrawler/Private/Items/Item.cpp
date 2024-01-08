

#include "Items/Item.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ItemMesh"));
}

TSubclassOf<AActor> AItem::Pickup(APawn* Pawn)
{
	return GetClass();
}

void AItem::SetSimulatePhysics(const bool bNewSimulatePhysics)
{
	ItemMesh->SetSimulatePhysics(bNewSimulatePhysics);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	ItemMesh->SetCollisionProfileName(FName("PickupItem_BlockVisibility"), true);
}
