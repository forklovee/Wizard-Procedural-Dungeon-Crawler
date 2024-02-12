// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class AKeyItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorClosed);

UCLASS()
class PROCDUNGEONCRAWLER_API ADoor : public AInteractiveObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnDoorOpened OnDoorOpened;
	UPROPERTY(BlueprintAssignable)
	FOnDoorClosed OnDoorClosed;
	
	ADoor();
	
	virtual void Interact(AHuman* HumanCharacter);
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION()
	void SetOpen(bool bNewIsOpen);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* FrameMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USceneComponent* DoorRoot;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* DoorMesh;	
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta=(AllowPrivateAccess = "true"))
	bool bIsOpen = false;
	
	

	bool bIsLocked = false;
};
