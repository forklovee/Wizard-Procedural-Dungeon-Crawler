

#include "Items/Item.h"

#include "Characters/Human/Human.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItem::Equip(AHuman* HumanPawn, USceneComponent* EquipTargetComponent, FName SocketName)
{
	
}

void AItem::UnEquip()
{
	
}

TSubclassOf<AActor> AItem::Pickup(APawn* Pawn)
{
	if (OnPickedUp.IsBound())
	{
		OnPickedUp.Broadcast();
	}
	
	return GetClass();
}
