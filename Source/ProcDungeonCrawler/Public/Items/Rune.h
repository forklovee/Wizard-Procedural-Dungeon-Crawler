// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupItem.h"
#include "GameFramework/Actor.h"
#include "Interface/PropPickupInterface.h"
#include "Rune.generated.h"

class URuneCast;

UCLASS()
class PROCDUNGEONCRAWLER_API ARune : public APickupItem
{
	GENERATED_BODY()
	
public:	
	TSoftObjectPtr<URuneCast>& GetRuneCast();

	virtual UDataAsset* GetAdditionalDataAsset_Implementation() override;

	virtual FName GetItemName_Implementation() override;
	virtual FName GetInteractionName_Implementation() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	TSoftObjectPtr<URuneCast> RuneCast;
};
