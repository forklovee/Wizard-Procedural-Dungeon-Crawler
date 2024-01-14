

#include "Items/Item.h"

#include "Components/CapsuleComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>(FName("WeaponRoot"));
	RootComponent = WeaponRoot;
	
	PickupCollision = CreateDefaultSubobject<UCapsuleComponent>(FName("PickupCollision"));
	PickupCollision->SetupAttachment(WeaponRoot);
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
