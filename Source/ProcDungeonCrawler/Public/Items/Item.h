// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem();
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	virtual TSubclassOf<AActor> Pickup(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category = "Physics")
	void SetSimulatePhysics(const bool bNewSimulatePhysics);

public:
	UPROPERTY(EditAnywhere, Category="ItemName")
	FName ItemName = "Item";
	
	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* ItemMesh;
	
protected:
	virtual void BeginPlay() override;
	
	FTimerHandle DestroyTimerHandle;
};
