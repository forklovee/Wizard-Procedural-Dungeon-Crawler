

#include "Items/PickupItem.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("ItemMesh"));
	RootComponent = ItemMesh;
	ItemMesh->SetCollisionProfileName("Item");
	ItemMesh->SetSimulatePhysics(true);
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

TSubclassOf<AActor> APickupItem::Pickup_Implementation(APawn* Pawn)
{
	IPropPickupInterface::Pickup_Implementation(Pawn);

	const TSubclassOf<AActor> ItemClass = GetClass();
	
	Destroy();
	
	return ItemClass;
}

UDataAsset* APickupItem::GetAdditionalDataAsset_Implementation()
{
	IPropPickupInterface::GetAdditionalDataAsset_Implementation();

	return nullptr;
}

FText APickupItem::GetPropNameText_Implementation()
{
	return FText::FromString( GetClass()->GetName() );
}

void APickupItem::Grab_Implementation()
{
	IPropGrabInterface::Grab_Implementation();

	if (OnItemGrabbedChanged.IsBound())
	{
		OnItemGrabbedChanged.Broadcast(this, true);
	}
	SetSimulatePhysics(true);
}

UPrimitiveComponent* APickupItem::GetGrabComponent_Implementation()
{
	IPropGrabInterface::GetGrabComponent_Implementation();

	return ItemMesh;
}

void APickupItem::Release_Implementation()
{
	IPropGrabInterface::Release_Implementation();
	if (OnItemGrabbedChanged.IsBound())
	{
		OnItemGrabbedChanged.Broadcast(this, false);
	}
}

void APickupItem::SetSimulatePhysics_Implementation(const bool bNewSimulatePhysics)
{
	IPropGrabInterface::SetSimulatePhysics_Implementation(bNewSimulatePhysics);

	if (ItemMesh == nullptr) return;

	ItemMesh->SetSimulatePhysics(bNewSimulatePhysics);
}

bool APickupItem::CanBeGrabbed_Implementation()
{
	IPropGrabInterface::CanBeGrabbed_Implementation();

	return bCanBeGrabbed;
}