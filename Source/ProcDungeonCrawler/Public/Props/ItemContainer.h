// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "GameFramework/Actor.h"
#include "ItemContainer.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API AItemContainer : public AInteractiveObject
{
	GENERATED_BODY()
	
public:	
	AItemContainer();

protected:
	virtual void BeginPlay() override;

	virtual void OnDestroyed_Implementation() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<class APickupItem>, int32> ItemsInside;
};
