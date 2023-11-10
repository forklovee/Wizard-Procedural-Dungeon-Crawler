// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BagComponent.generated.h"

class APickupItem;
class ABagActor;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBagContentsUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBagStateChanged, bool, bIsOpen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeftRightInput, int, Direction);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UBagComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FOnBagContentsUpdated OnBagContentsUpdated;
	FOnBagStateChanged OnBagStateChanged;

	FOnPlayerLeftRightInput OnPlayerLeftRightInput;
	
	UBagComponent();

	UFUNCTION(BlueprintCallable, Category= "Bag")
	bool IsOpen() const;
	
	UFUNCTION(BlueprintCallable, Category= "Bag")
	void SetBagActorAttached(const bool bIsAttached);
	
	UFUNCTION(BlueprintCallable, Category= "Bag")
	void ToggleBag();

	UFUNCTION(BlueprintCallable, Category= "Bag")
	void SetupBagActor();
	
	UFUNCTION()
	void OnLeftRightInputAction(int Direction);
	
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
