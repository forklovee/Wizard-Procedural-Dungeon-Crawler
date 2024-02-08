#include "Items/Consumables/ConsumableItem.h"
#include "Components/Character/PawnStats.h"
#include "Characters/Human/Human.h"

AConsumableItem::AConsumableItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AConsumableItem::Use(AHuman* HumanPawn)
{
	UE_LOG(LogTemp, Display, TEXT("Dupa"))
	if (HealthChange != 0.f)
	{
		HumanPawn->Stats->ChangeHealth(HealthChange);
	}
	if (ManaChange != 0.f)
	{
		HumanPawn->Stats->ChangeMana(ManaChange);
	}
}
