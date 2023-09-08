// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PropPickupInterface.h"
#include "RunePickup.generated.h"

class URuneCast;

UCLASS()
class PROCDUNGEONCRAWLER_API ARunePickup : public AActor, public IPropPickupInterface
{
	GENERATED_BODY()
	
public:	
	ARunePickup();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	AActor* Pickup(APawn* Pawn);
	virtual AActor* Pickup_Implementation(APawn* Pawn) override;
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	TSoftObjectPtr<URuneCast> RuneCast;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune|Mesh")
	UStaticMeshComponent* RuneStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune|Mesh")
	USkeletalMeshComponent* RuneSkeletalMesh;

	/* 
	 * Tick true if you want the rune to use skeletal mesh with its animations
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune|Mesh")
	bool bUseSkeletalMesh = false;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* PickupCollision;

};
