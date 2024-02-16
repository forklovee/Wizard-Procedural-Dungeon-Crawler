
#include "Components/Character/PlayerInteractionRaycast.h"

#include "Characters/Human/Human.h"
#include "Components/Props/InteractionShapeComponent.h"
#include "Items/PickupItem.h"
#include "Items/Clothes/Clothes.h"
#include "Items/Clothes/RuneRing.h"
#include "Spell/Rune.h"
#include "Items/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Props/InteractiveObject.h"

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
		if (OnNewInteractionTarget.IsBound())
		{
			OnNewInteractionTarget.Broadcast(FText::FromString(NewItem->GetName()), FName("Interact"), Cast<AClothes>(NewItem) == nullptr);
		}
		
		return;
	}
	
	if (AInteractiveObject* NewInteractionObject = Cast<AInteractiveObject>(NewInteractionTarget.GetActor())) // new interaction target
	{
		if (!NewInteractionTarget.GetComponent()->ComponentTags.Contains("InteractiveObject"))
		{
			return;
		}
		if (OnNewInteractionTarget.IsBound())
		{
			OnNewInteractionTarget.Broadcast(FText::FromString(NewInteractionObject->GetName()), FName("Interact"), false);
		}
		
		return;
	}
	
	NewInteractionTarget = FHitResult();
	if (OnNewInteractionTarget.IsBound())
	{
		OnNewInteractionTarget.Broadcast(FText::FromString(""), FName(), false);
	}
}

AActor* UPlayerInteractionRaycast::GetInteractionTarget() const
{
	return InteractionTarget.GetActor();
}

void UPlayerInteractionRaycast::Interact()
{
	USceneComponent* TargetComponent = InteractionTarget.GetComponent();
	AActor* Target = InteractionTarget.GetActor();
	ClearInteractionTarget();
	
	if (Target == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Interaction Target is null"))
		return;
	}

	if (AItem* Item = Cast<AItem>(Target))
	{
		PickUpItem(Item);
		return;
	}

	if (AInteractiveObject* InteractiveObject = Cast<AInteractiveObject>(Target))
	{
		InteractiveObject->Interact(Cast<AHuman>(GetOwner()));
	}
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
	
	// if (AWeapon* Weapon = Cast<AWeapon>(Item))
	// {
	// 	if (OnWeaponPickedUp.IsBound())
	// 	{
	// 		OnWeaponPickedUp.Broadcast(Weapon);
	// 	}
	// 	return true;
	// }

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
		OnItemPickedUp.Broadcast(Item, 1);
	}
	
	return true;
}
