// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickedUp);

UCLASS()
class AItem : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;
	
	AItem();
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	virtual TSubclassOf<AActor> Pickup(APawn* Pawn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UCapsuleComponent* PickupCollision;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName ItemName = "Item";
	
	FTimerHandle DestroyTimerHandle;
};
