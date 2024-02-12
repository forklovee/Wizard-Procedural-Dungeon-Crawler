#include "Props/InteractiveObject.h"

AInteractiveObject::AInteractiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AInteractiveObject::BeginPlay()
{
	Super::BeginPlay();

	OnInteractionSuccess.AddDynamic(this, &AInteractiveObject::OnInteractionSuccessEvent);
	OnInteractionFail.AddDynamic(this, &AInteractiveObject::OnInteractionFailEvent);
	
	OnTakeAnyDamage.AddDynamic(this, &AInteractiveObject::OnDamageTaken);
}

void AInteractiveObject::Interact(AHuman* HumanCharacter)
{
	EInteractionFailReason FailReason;
	if (CanInteract(HumanCharacter, FailReason))
	{
		if (OnInteractionSuccess.IsBound())
		{
			OnInteractionSuccess.Broadcast(HumanCharacter);
		}
	}
	else
	{
		if (OnInteractionFail.IsBound())
		{
			OnInteractionFail.Broadcast(HumanCharacter, FailReason);
		}
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

void AInteractiveObject::OnInteractionSuccessEvent_Implementation(AHuman* HumanCharacter)
{
	
}

void AInteractiveObject::OnInteractionFailEvent_Implementation(AHuman* HumanCharacter, EInteractionFailReason FailReason)
{
	
}
