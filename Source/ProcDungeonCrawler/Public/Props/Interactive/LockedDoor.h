// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Props/Interactive/Door.h"
#include "LockedDoor.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API ALockedDoor : public ADoor
{
	GENERATED_BODY()

public:
	ALockedDoor();
	
	virtual bool Interact_Implementation(APawn* Pawn) override;

protected:
	virtual void BeginPlay() override;
	
private:
	bool CanPawnUnlock(APawn* Pawn) const;

	UFUNCTION()
	void OnLockUnlockTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										 const FHitResult& SweepResult);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockedDoor")
	TSubclassOf<class ADoorKey> RequiredKeyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockedDoor")
	UBoxComponent* LockUnlockTrigger;

private:
	bool bIsLocked = true;
};
