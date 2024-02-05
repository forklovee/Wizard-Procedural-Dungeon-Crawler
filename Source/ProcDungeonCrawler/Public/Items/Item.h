// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class AHuman;
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

	UFUNCTION(BlueprintCallable, Category = "Usage")
	virtual void Use(AHuman* HumanPawn);
	
	UFUNCTION(BlueprintCallable, Category = "Equip")
	virtual void Equip(AHuman* HumanPawn, USceneComponent* EquipTargetComponent, FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "Equip")
	virtual void UnEquip();
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	virtual TSubclassOf<AActor> Pickup(APawn* Pawn);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FName ItemName = "Item";
	
	FTimerHandle DestroyTimerHandle;
};
