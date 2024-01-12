

#include "Items/Item.h"

#include "Components/CapsuleComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("PickupCollision"));
	RootComponent = PickupCollision;
}

TSubclassOf<AActor> AItem::Pickup(APawn* Pawn)
{
	if (OnPickedUp.IsBound())
	{
		OnPickedUp.Broadcast();
	}
	
	return GetClass();
}