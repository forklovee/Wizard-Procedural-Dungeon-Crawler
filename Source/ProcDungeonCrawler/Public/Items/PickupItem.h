// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PropInteractionEnabler.h"
#include "GameFramework/Actor.h"
#include "Interface/PropGrabInterface.h"
#include "Interface/PropPickupInterface.h"
#include "PickupItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemGrabbedChanged, APickupItem*, Item, bool, bIsGrabbed);

UCLASS()
class APickupItem : public AActor,
					public IPropInteractionEnabler, // Can be interaction target
					public IPropPickupInterface, public IPropGrabInterface // Can be picked up and grabbed
{
	GENERATED_BODY()
	
public:
	FOnItemGrabbedChanged OnItemGrabbedChanged;
	
	APickupItem();

	// Interaction Enabler interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FText GetPropNameText();
	virtual FText GetPropNameText_Implementation() override;
	
	//Pickup interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	TSubclassOf<AActor> Pickup(APawn* Pawn);
	virtual TSubclassOf<AActor> Pickup_Implementation(APawn* Pawn) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pickup")
	UDataAsset* GetAdditionalDataAsset();
	virtual UDataAsset* GetAdditionalDataAsset_Implementation() override;
	
	//Grab interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab")
	void Grab();
	virtual void Grab_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab")
	UPrimitiveComponent* GetGrabComponent();
	virtual UPrimitiveComponent* GetGrabComponent_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab")
	void Release();
	virtual void Release_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab")
	bool CanBeGrabbed();
	virtual bool CanBeGrabbed_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab")
	void SetSimulatePhysics(const bool bNewSimulatePhysics);
	virtual void SetSimulatePhysics_Implementation(const bool bNewSimulatePhysics);

public:
	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* ItemMesh;
	
protected:
	virtual void BeginPlay() override;
	
	FTimerHandle DestroyTimerHandle;
	bool bCanBeGrabbed = true;
};
