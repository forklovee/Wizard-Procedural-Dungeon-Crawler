// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/PickupItem.h"
//Is Door.h really needed???
#include "Props/Interactive/Door.h"

#include "DoorKey.generated.h"

class USphereComponent;
class ADoor;

UCLASS()
class PROCDUNGEONCRAWLER_API ADoorKey : public APickupItem
{
	GENERATED_BODY()

public:
	ADoorKey();

protected:
	virtual void BeginPlay() override;

	virtual void Grab_Implementation() override;
	virtual void Release_Implementation() override;
	
	UFUNCTION()
	void OnUnlockTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									 const FHitResult& SweepResult);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	USphereComponent* UnlockTrigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	bool bIsUniversal = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	TWeakObjectPtr<ADoor> TargetDoor;

private:
	bool bIsUnlockTriggerConnected = false;
};
