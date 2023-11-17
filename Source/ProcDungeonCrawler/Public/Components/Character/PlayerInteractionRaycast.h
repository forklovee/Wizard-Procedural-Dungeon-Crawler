// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerInteractionRaycast.generated.h"

class AWeapon;
class URuneCast;
class APickupItem;

UENUM(BlueprintType)
enum class EInteractionType
{
	E_None = 0,
	E_Pickup = 1 << 1,
	E_Grab = 1 << 2,
	E_Interact = 1 << 3,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNewInteractionTarget, FText, ActorName, EInteractionType, OnClickInteractionType, bool, bCanBeGrabbed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponPickedUp, TSubclassOf<AWeapon>, WeaponClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemPickedUp, TSubclassOf<APickupItem>, ItemClass, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunePickedUp, URuneCast*, RuneCast);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPropGrabbed, UPrimitiveComponent*, GrabComponent, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPropReleased, UPrimitiveComponent*, GrabComponent, AActor*, Actor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UPlayerInteractionRaycast : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewInteractionTarget OnNewInteractionTarget;
	
	UPROPERTY(BlueprintAssignable)
	FOnItemPickedUp OnItemPickedUp;
	UPROPERTY(BlueprintAssignable)
	FOnRunePickedUp OnRunePickedUp;
	UPROPERTY(BlueprintAssignable)
	FOnWeaponPickedUp OnWeaponPickedUp;
	
	UPROPERTY(BlueprintAssignable)
	FOnPropGrabbed OnPropGrabbed;
	UPROPERTY(BlueprintAssignable)
	FOnPropReleased OnPropReleased;
	
	UPlayerInteractionRaycast();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionTarget(FVector NewForwardVector);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Interact();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UPrimitiveComponent* Grab();
	bool CanGrabTarget() const;
	FTransform GetGrabTargetTransform() const;
	bool IsGrabbingItem() const;
	AActor* GetGrabbedActor() const;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Release();
	
	UFUNCTION()
	void SetGrabbedActorPositionOverride(FVector NewPosition);
	UFUNCTION()
	void ClearGrabbedActorPositionOverride();
private:
	void CastRaycast(FHitResult& OutHitResult) const;
	void ClearInteractionTarget() const;
	
	bool PickUpItem(AActor* Target) const;
	
	EInteractionType GetOnClickInteractionType(const AActor* Actor) const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bInteractionDisabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bGrabDisabled = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float RaycastLength = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float RaycastRadius = 5.f;

private:
	FVector ForwardVector;
	
	bool bUseGrabOverridenPosition = false;
	FVector GrabbedActorPositionOverride;
	
	TWeakObjectPtr<AActor> GrabbedActor;
	FHitResult InteractionTarget;
};

