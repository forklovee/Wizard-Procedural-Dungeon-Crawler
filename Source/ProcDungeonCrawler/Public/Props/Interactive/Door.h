// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PropInteractionEnabler.h"
#include "GameFramework/Actor.h"
#include "Interface/PropInteractionInterface.h"
#include "Door.generated.h"

class UPhysicsConstraintComponent;
class UActorSequenceComponent;

UCLASS()
class PROCDUNGEONCRAWLER_API ADoor : public AActor,
								public IPropInteractionEnabler,
								public IPropInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ADoor();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FText GetPropNameText();
	virtual FText GetPropNameText_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool Interact(APawn* Pawn);
	virtual bool Interact_Implementation(APawn* Pawn) override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionNameText();
	virtual FText GetInteractionNameText_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	bool IsDoorOpened() const;
	
	void OpenDoor(const APawn* Pawn);
	void CloseDoor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UPhysicsConstraintComponent* DoorConstraint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* DoorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* FrameMesh;
	
private:
	int OpenDirection = 0;
	int LastOpenDirection = 0;
	
	float DoorOpenFinalYaw = 0.0f;
	
};
