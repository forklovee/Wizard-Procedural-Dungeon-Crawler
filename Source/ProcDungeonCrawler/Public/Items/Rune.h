// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "GameFramework/Actor.h"
#include "Interface/PropPickupInterface.h"
#include "Rune.generated.h"

class URuneCast;

UCLASS()
class PROCDUNGEONCRAWLER_API ARune : public AActor,
									public IPropInteractionEnabler, // Can be interaction target
									public IPropPickupInterface // Can be picked up
{
	GENERATED_BODY()
	
public:
	ARune();
	TSoftObjectPtr<URuneCast>& GetRuneCast();

	// Interaction Enabler Interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	FText GetPropNameText();
	virtual FText GetPropNameText_Implementation() override;

	// Pickup Interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	TSubclassOf<AActor> Pickup(APawn* Pawn);
	TSubclassOf<AActor> Pickup_Implementation(APawn* Pawn);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	UDataAsset* GetAdditionalDataAsset();
	virtual UDataAsset* GetAdditionalDataAsset_Implementation() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	TSoftObjectPtr<URuneCast> RuneCast;
};
