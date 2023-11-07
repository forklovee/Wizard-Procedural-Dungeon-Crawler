// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BagComponent.generated.h"

class APickupItem;
class ABagActor;
struct FInputActionValue;
DECLARE_MULTICAST_DELEGATE(FOnBagUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UBagComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FOnBagUpdated OnBagUpdated;
	
	UBagComponent();

	bool IsOpen() const;

	UFUNCTION()
	void ToggleBag();
	
	void AddItem(TSubclassOf<APickupItem> ItemClass, int32 Amount = 1);
	void RemoveItem(TSubclassOf<APickupItem> ItemClass, int32 Amount = 1);
	
	bool HasItemClass(TSubclassOf<APickupItem> ItemClass) const;
	int32 GetItemAmount(TSubclassOf<APickupItem> ItemClass) const;

public:
	// Bag class
	UPROPERTY(EditAnywhere, Category= "Bag")
	TSubclassOf<ABagActor> BagActorClass;
	
private:
	UPROPERTY()
	TMap<TSubclassOf<APickupItem>, int32> Items;

	// 3D UI Actor
	TWeakObjectPtr<ABagActor> BagActor;
};
