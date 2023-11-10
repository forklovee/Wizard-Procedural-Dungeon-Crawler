// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PropPickupInterface.h"
#include "PickupItem.generated.h"

UCLASS()
class APickupItem : public AActor, public IPropPickupInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

	void SetSimulatePhysics(const bool bNewSimulatePhysics);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	TSubclassOf<APickupItem> Pickup(APawn* Pawn);
	virtual TSubclassOf<APickupItem> Pickup_Implementation(APawn* Pawn) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	UDataAsset* GetAdditionalDataAsset();
	virtual UDataAsset* GetAdditionalDataAsset_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	FName GetItemName();
	virtual FName GetItemName_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	FName GetInteractionName();
	virtual FName GetInteractionName_Implementation() override;

	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* ItemMesh;
	
protected:
	virtual void BeginPlay() override;
	
	FTimerHandle DestroyTimerHandle;
};
