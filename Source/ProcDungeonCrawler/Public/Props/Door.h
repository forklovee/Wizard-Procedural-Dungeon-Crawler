// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveObject.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UInteractionShapeComponent;
class AKeyItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpened, AHuman*, HumanCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorClosed, AHuman*, HumanCharacter);

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

	virtual bool Interact(AHuman* HumanCharacter) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void InteractionSuccess(AHuman* HumanCharacter) override;
	virtual void InteractionFailed(AHuman* HumanCharacter) override;

public:
	UFUNCTION()
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION()
	void SetOpen(bool bNewIsOpen, AHuman* HumanCharacter);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AKeyItem> RequiredKeyClass = nullptr;
};
