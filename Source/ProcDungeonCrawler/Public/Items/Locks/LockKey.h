// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItem.h"
#include "LockKey.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API ALockKey : public APickupItem
{
	GENERATED_BODY()

public:
	ALockKey();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USphereComponent* UnlockTrigger;

protected:
	bool bCanUnlock = false;
};
