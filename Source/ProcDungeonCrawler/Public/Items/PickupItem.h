// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "PickupItem.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API APickupItem : public AItem
{
	GENERATED_BODY()

public:
	APickupItem();

	UFUNCTION(BlueprintCallable, Category = "Physics")
	void SetSimulatePhysics(const bool bNewSimulatePhysics);


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* ItemMesh;
	
protected:
	virtual void BeginPlay() override;
};
