#include "Props/InteractiveObject.h"

AInteractiveObject::AInteractiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AInteractiveObject::Interact(AHuman* HumanCharacter)
{
	return true;
}

void AInteractiveObject::BeginPlay()
{
	Super::BeginPlay();
	
	OnTakeAnyDamage.AddDynamic(this, &AInteractiveObject::OnDamageTaken);
}

void AInteractiveObject::InteractionSuccess(AHuman* HumanCharacter)
{
	if (OnInteractionSuccess.IsBound())
	{
		OnInteractionSuccess.Broadcast(HumanCharacter);
	}
}

void AInteractiveObject::InteractionFailed(AHuman* HumanCharacter)
{
	if (OnInteractionFail.IsBound())
	{
		OnInteractionFail.Broadcast(HumanCharacter);
	}
}

void AInteractiveObject::OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bCanBeDestroyed || Durability <= 0.f) return;

	Durability -= Damage;
	if (Durability <= 0)
	{
		OnDestroyed();
	}
}

void AInteractiveObject::OnDestroyed_Implementation()
{
	if (OnObjectDestroyed.IsBound())
	{
		OnObjectDestroyed.Broadcast();
	}
	
	Destroy();
}

bool AInteractiveObject::CanInteract(AHuman* HumanCharacter, EInteractionFailReason& OutFailReason) const
{
	return true;
}
