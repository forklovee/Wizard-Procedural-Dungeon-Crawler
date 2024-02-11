// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Clothes.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EArmorTarget: uint8
{
	Head,
	Chest,
	Feet,
	Hands
};

UCLASS()
class PROCDUNGEONCRAWLER_API AClothes : public AItem
{
	GENERATED_BODY()

public:
	AClothes();

	
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Clothes")
	EArmorTarget ArmorTarget = EArmorTarget::Chest;
};
