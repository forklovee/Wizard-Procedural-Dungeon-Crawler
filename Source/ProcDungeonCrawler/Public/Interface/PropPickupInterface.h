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
	TSubclassOf<APickupItem> Pickup(APawn* Pawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	UDataAsset* GetAdditionalDataAsset();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	FName GetItemName();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	FName GetInteractionName();
};
