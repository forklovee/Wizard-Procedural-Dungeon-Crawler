// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItem.h"
#include "ConsumableItem.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API AConsumableItem : public APickupItem
{
	GENERATED_BODY()

public:
	AConsumableItem();

public:
	virtual void Use(AHuman* HumanPawn) override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Consumable")
	float HealthChange = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Consumable")
	float ManaChange = 0.f;
};
