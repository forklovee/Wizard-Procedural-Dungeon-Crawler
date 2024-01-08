// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerInteractionRaycast.generated.h"

class AItem;
class AWeapon;
class URune;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNewInteractionTarget, FText, ActorName, FName, InteractionType, bool, bCanBeGrabbed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponPickedUp, TSubclassOf<AWeapon>, WeaponClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemPickedUp, TSubclassOf<AItem>, ItemClass, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunePickedUp, URune*, RuneCast);

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
	
	UPlayerInteractionRaycast();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionTarget(FVector NewForwardVector);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Interact();

private:
	void CastRaycast(FHitResult& OutHitResult) const;
	void ClearInteractionTarget() const;
	
	bool PickUpItem(AItem* Item) const;
	
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

