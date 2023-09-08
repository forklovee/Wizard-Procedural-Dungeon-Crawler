// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PropPickupInterface.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickup, APawn*, Pawn);

UINTERFACE(MinimalAPI)
class UPropPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROCDUNGEONCRAWLER_API IPropPickupInterface
{
	GENERATED_BODY()

public:
	FOnPickup OnPickup;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	AActor* Pickup(APawn* Pawn);
};
