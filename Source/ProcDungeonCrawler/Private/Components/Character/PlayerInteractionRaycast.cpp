
#include "Components/Character/PlayerInteractionRaycast.h"

#include "Components/PropInteractionEnabler.h"
#include "Interface/PropGrabInterface.h"
#include "Interface/PropInteractionInterface.h"
#include "Interface/PropPickupInterface.h"
#include "Items/PickupItem.h"
#include "Items/Rune.h"
#include "Items/Weapon/Weapon.h"
#include "Spell/RuneCast.h"
#include "Kismet/KismetSystemLibrary.h"

UPlayerInteractionRaycast::UPlayerInteractionRaycast()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerInteractionRaycast::UpdateInteractionTarget(FVector NewForwardVector)
{
	ForwardVector = NewForwardVector;
	
	if (IsGrabbingItem() || (bInteractionDisabled && bGrabDisabled))
	{
		return;
	}
	
	FHitResult NewInteractionTarget;
	CastRaycast(NewInteractionTarget);
	
	if (NewInteractionTarget.GetActor() != nullptr &&
		!NewInteractionTarget.GetActor()->Implements<UPropInteractionEnabler>()
		)
	{
		NewInteractionTarget = FHitResult();
	}

	if (NewInteractionTarget.GetActor() != InteractionTarget.GetActor() && OnNewInteractionTarget.IsBound())
	{
		// No Valid interaction target
		if (NewInteractionTarget.GetActor() == nullptr)
		{
			InteractionTarget = NewInteractionTarget;
			OnNewInteractionTarget.Broadcast(FText::FromString(""), EInteractionType::E_None, false);
			return;
		}
		
		const FText ActorNameText = IPropInteractionEnabler::Execute_GetPropNameText(NewInteractionTarget.GetActor());
		
		// Get possible interaction types
		const EInteractionType OnClickInteractionType = GetOnClickInteractionType(NewInteractionTarget.GetActor());
		const bool bActorCanBeGrabbed = NewInteractionTarget.GetActor()->Implements<UPropGrabInterface>();

		// New interaction target
		if (OnClickInteractionType != EInteractionType::E_None || bActorCanBeGrabbed)
		{
			InteractionTarget = NewInteractionTarget;
			OnNewInteractionTarget.Broadcast(ActorNameText, OnClickInteractionType, bActorCanBeGrabbed);
			return;
		}

		UE_LOG(LogTemp, Error, TEXT("No valid interaction targets 2"));

		// New interaction target is not valid.
		InteractionTarget = NewInteractionTarget;
		OnNewInteractionTarget.Broadcast(FText::FromString(""), EInteractionType::E_None, false);
	}
}

bool UPlayerInteractionRaycast::Interact()
{
	AActor* Target = InteractionTarget.GetActor();
	
	if (Target == nullptr || !Target->Implements<UPropInteractionEnabler>())
	{
		return false;
	}
	
	if (Target->Implements<UPropPickupInterface>())
	{
		ClearInteractionTarget();
		
		return PickUpItem(Target);
	}
	if (Target->Implements<UPropInteractionInterface>())
	{
		ClearInteractionTarget();

		return IPropInteractionInterface::Execute_Interact(Target, Cast<APawn>(GetOwner()));;
	}
	
	return false;
}

void UPlayerInteractionRaycast::GrabInteractionTarget()
{
	AActor* Target = InteractionTarget.GetActor();
	
	if (Target == nullptr ||
		!Target->Implements<UPropInteractionEnabler>() ||
		!Target->Implements<UPropGrabInterface>())
	{
		return;
	}
	
	if (IPropGrabInterface::Execute_CanBeGrabbed(Target))
	{
		GrabbedActor = Target;
			
		IPropGrabInterface::Execute_Grab(Target);
		UPrimitiveComponent* GrabComponent = IPropGrabInterface::Execute_GetGrabComponent(Target);
		if (OnPropGrabbed.IsBound())
		{
			OnPropGrabbed.Broadcast(GrabComponent, Target);
		}
	}
}

bool UPlayerInteractionRaycast::CanGrabTarget() const
{
	return InteractionTarget.GetActor() != nullptr &&
		   InteractionTarget.GetActor()->Implements<UPropInteractionEnabler>() &&
		   InteractionTarget.GetActor()->Implements<UPropGrabInterface>() &&
		   IPropGrabInterface::Execute_CanBeGrabbed(InteractionTarget.GetActor());
}

FTransform UPlayerInteractionRaycast::GetGrabTargetTransform() const
{
	FVector GrabLocation;
	if (bUseGrabOverridenPosition)
	{
		const FRotator LookAtRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator();
		GrabLocation = GrabbedActorPositionOverride;
		return FTransform(LookAtRotator, GrabLocation);
	}

	FHitResult NewInteractionTarget;
	CastRaycast(NewInteractionTarget);
	float GrabDistance = NewInteractionTarget.GetActor() ? NewInteractionTarget.Distance - 55.f: 100.f;
	GrabDistance = FMath::Clamp(GrabDistance, 50.f, 100.f);
	GrabLocation = GetComponentLocation() + ForwardVector * GrabDistance;
	const FRotator LookAtRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator();
	return FTransform(LookAtRotator, GrabLocation);
}

bool UPlayerInteractionRaycast::IsGrabbingItem() const
{
	return GrabbedActor.IsValid();
}

AActor* UPlayerInteractionRaycast::GetGrabbedActor() const
{
	return GrabbedActor.Get();
}

void UPlayerInteractionRaycast::Release()
{
	AActor* Target = GrabbedActor.Get();
	GrabbedActor = nullptr;
	
	if (Target == nullptr ||
		!Target->Implements<UPropInteractionEnabler>() ||
		!Target->Implements<UPropGrabInterface>())
	{
		return;
	}
	bUseGrabOverridenPosition = false;
	IPropGrabInterface::Execute_Release(Target);
	UPrimitiveComponent* GrabComponent = IPropGrabInterface::Execute_GetGrabComponent(Target);
	if (OnPropReleased.IsBound())
	{
		OnPropReleased.Broadcast(GrabComponent, Target);
	}
}

void UPlayerInteractionRaycast::SetGrabbedActorPositionOverride(FVector NewPosition)
{
	bUseGrabOverridenPosition = true;
	GrabbedActorPositionOverride = NewPosition;
}

void UPlayerInteractionRaycast::ClearGrabbedActorPositionOverride()
{
	bUseGrabOverridenPosition = false;
}

void UPlayerInteractionRaycast::CastRaycast(FHitResult& OutHitResult) const
{
	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		GetComponentLocation(),
		GetComponentLocation() + ForwardVector * RaycastLength,
		RaycastRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{},
		EDrawDebugTrace::None,
		OutHitResult,
		true
	);
}

void UPlayerInteractionRaycast::ClearInteractionTarget() const
{
	if (OnNewInteractionTarget.IsBound())
	{
		OnNewInteractionTarget.Broadcast(FText::FromString(""), EInteractionType::E_None, false);
	}
}

bool UPlayerInteractionRaycast::PickUpItem(AActor* Target) const
{
	if (const AWeapon* Weapon = Cast<AWeapon>(Target))
	{
		if (OnWeaponPickedUp.IsBound())
		{
			OnWeaponPickedUp.Broadcast(Weapon->GetClass());
		}
		IPropPickupInterface::Execute_Pickup(Target, Cast<APawn>(GetOwner()));
		return true;
	}

	if (const APickupItem* PickupItem = Cast<APickupItem>(Target))
	{
		if (OnItemPickedUp.IsBound())
		{
			OnItemPickedUp.Broadcast(PickupItem->GetClass(), 1);
		}

		IPropPickupInterface::Execute_Pickup(Target, Cast<APawn>(GetOwner()));
		return true;
	}

	if (ARune* Rune = Cast<ARune>(Target))
	{
		UDataAsset* AdditionalDataAsset = IPropPickupInterface::Execute_GetAdditionalDataAsset(Rune);
		if (URuneCast* RuneCast = Cast<URuneCast>(AdditionalDataAsset))
		{
			if (OnRunePickedUp.IsBound())
			{
				OnRunePickedUp.Broadcast(RuneCast);
			}
		}
		IPropPickupInterface::Execute_Pickup(Target, Cast<APawn>(GetOwner()));
		return true;
	}
	return false;
}

EInteractionType UPlayerInteractionRaycast::GetOnClickInteractionType(const AActor* Actor) const
{
	if (bInteractionDisabled || Actor == nullptr || !Actor->Implements<UPropInteractionEnabler>())
	{
		return EInteractionType::E_None;
	}

	if (Actor->Implements<UPropPickupInterface>())
	{
		return EInteractionType::E_Pickup;
	}
	if (Actor->Implements<UPropInteractionInterface>())
	{
		return EInteractionType::E_Interact;
	}
	
	return EInteractionType::E_None;
}
