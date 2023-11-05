

#include "Items/PickupItem.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
}

TSubclassOf<APickupItem> APickupItem::Pickup_Implementation(APawn* Pawn)
{
	IPropPickupInterface::Pickup_Implementation(Pawn);

	const TSubclassOf<APickupItem> ItemClass = GetClass();
	
	Destroy();
	
	return ItemClass;
}

UDataAsset* APickupItem::GetAdditionalDataAsset_Implementation()
{
	IPropPickupInterface::GetAdditionalDataAsset_Implementation();

	return nullptr;
}

FName APickupItem::GetItemName_Implementation()
{
	return FName(GetName());
}

FName APickupItem::GetInteractionName_Implementation()
{
	return FName("Pick up");
}