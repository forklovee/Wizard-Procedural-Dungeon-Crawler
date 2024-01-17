
#include "Components/Character/PlayerInteractionRaycast.h"
#include "Components/Props/InteractionShapeComponent.h"
#include "Items/PickupItem.h"
#include "Items/Clothes/Clothes.h"
#include "Items/Clothes/RuneRing.h"
#include "Spell/Rune.h"
#include "Items/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"

UPlayerInteractionRaycast::UPlayerInteractionRaycast()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerInteractionRaycast::UpdateInteractionTarget(FVector NewForwardVector)
{
	ForwardVector = NewForwardVector;
	
	// if (bInteractionDisabled && bGrabDisabled)
	// {
	// 	return;
	// }
	
	FHitResult NewInteractionTarget;
	CastRaycast(NewInteractionTarget);

	if (NewInteractionTarget.GetActor() == InteractionTarget.GetActor()) // same target
	{
		return;
	}
	InteractionTarget = NewInteractionTarget;
	
	if (AItem* NewItem = Cast<AItem>(NewInteractionTarget.GetActor())) // new target is an item
	{
		UE_LOG(LogTemp, Display, TEXT("New Interaction Target! %s"), *NewItem->GetName())

		if (OnNewInteractionTarget.IsBound())
		{
			OnNewInteractionTarget.Broadcast(FText::FromString(NewInteractionTarget.GetActor()->GetName()), FName("Interact"), Cast<AClothes>(NewItem) == nullptr);
		}
		
		return;
	}
	
	if (Cast<UInteractionShapeComponent>(NewInteractionTarget.GetComponent()) != nullptr) // new interaction target
	{
		if (OnNewInteractionTarget.IsBound())
		{
			OnNewInteractionTarget.Broadcast(FText::FromString(NewInteractionTarget.GetActor()->GetName()), FName("Interact"), false);
		}
		
		return;
	}
	
	NewInteractionTarget = FHitResult();
	UE_LOG(LogTemp, Display, TEXT("Clear Interaction Target!"))
	if (OnNewInteractionTarget.IsBound())
	{
		OnNewInteractionTarget.Broadcast(FText::FromString(""), FName(), false);
	}
}

AActor* UPlayerInteractionRaycast::GetInteractionTarget() const
{
	return InteractionTarget.GetActor();
}

bool UPlayerInteractionRaycast::Interact()
{
	USceneComponent* TargetComponent = InteractionTarget.GetComponent();
	AActor* Target = InteractionTarget.GetActor();

	ClearInteractionTarget();
	
	if (Target == nullptr) return false;

	if (AItem* Item = Cast<AItem>(Target))
	{
		return PickUpItem(Item);
	}

	if (UInteractionShapeComponent* InteractionShapeComponent = Cast<UInteractionShapeComponent>(TargetComponent))
	{
		return InteractionShapeComponent->Interact(Cast<APawn>(GetOwner()));
	}
	
	return false;
}

void UPlayerInteractionRaycast::CastRaycast(FHitResult& OutHitResult) const
{
	FHitResult VisibilityCollisionHitResult;
	UKismetSystemLibrary::SphereTraceSingle(
			GetWorld(),
			GetComponentLocation(),
			GetComponentLocation() + ForwardVector * RaycastLength,
			RaycastRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			{GetOwner()},
			EDrawDebugTrace::None,
			VisibilityCollisionHitResult,
			true,
			FLinearColor::Gray,
		FLinearColor::Blue,
		1.f
		);

	if (VisibilityCollisionHitResult.GetActor() != nullptr)
	{
		OutHitResult = VisibilityCollisionHitResult;
		return;
	}
	
	UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		GetComponentLocation(),
		GetComponentLocation() + ForwardVector * RaycastLength,
		RaycastRadius,
		UEngineTypes::ConvertToTraceType(ECC_EngineTraceChannel2),
		false,
		{GetOwner()},
		EDrawDebugTrace::None,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.f
	);
}

void UPlayerInteractionRaycast::ClearInteractionTarget() const
{
	if (OnNewInteractionTarget.IsBound())
	{
		OnNewInteractionTarget.Broadcast(FText::FromString(""), FName(""), false);
	}
}

bool UPlayerInteractionRaycast::PickUpItem(AItem* Item) const
{
	if (Item == nullptr) return false;
	
	Item->Pickup( Cast<APawn>(GetOwner()) );
	
	if (AWeapon* Weapon = Cast<AWeapon>(Item))
	{
		if (OnWeaponPickedUp.IsBound())
		{
			OnWeaponPickedUp.Broadcast(Weapon->GetClass());
		}
		return true;
	}

	if (ARuneRing* Rune = Cast<ARuneRing>(Item))
	{
		if (URune* RuneCast = Rune->GetRuneCast().LoadSynchronous())
		{
			if (OnRunePickedUp.IsBound())
			{
				OnRunePickedUp.Broadcast(RuneCast);
			}
		}
		Rune->Pickup(Cast<APawn>(GetOwner()));
		return true;
	}
	
	if (OnItemPickedUp.IsBound())
	{
		OnItemPickedUp.Broadcast(Item->GetClass(), 1);
	}
	
	return true;
}
