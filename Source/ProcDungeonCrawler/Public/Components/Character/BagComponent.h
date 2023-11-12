// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BagComponent.generated.h"

class AWizardPlayer;
class UWidgetComponent;
class APickupItem;
class ABagActor;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBagContentsUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBagStateChanged, bool, bIsOpen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCursorHoverChanged, bool, bIsHovered, AActor*, PlayerGrabbedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeftRightInput, int, Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewItemAdded, TSubclassOf<APickupItem>, ItemClass, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, TSubclassOf<APickupItem>, ItemClass, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAmountChanged, TSubclassOf<APickupItem>, ItemClass, int32, NewAmount);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UBagComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Bag
	FOnBagContentsUpdated OnBagContentsUpdated;
	FOnBagStateChanged OnBagStateChanged;
	// Input
	FOnPlayerCursorHoverChanged OnPlayerCursorHoverChanged;
	FOnPlayerLeftRightInput OnPlayerLeftRightInput;
	// Items
	FOnNewItemAdded OnNewItemAdded;
	FOnItemRemoved OnItemRemoved;
	
	UBagComponent();

	UFUNCTION()
	void OnPlayerCursorOnWidgetHoverChanged(UWidgetComponent* WidgetComponent, UWidgetComponent* PreviousWidgetComponent);
	
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

	UFUNCTION()
	void AddItem(TSubclassOf<APickupItem> ItemClass, int32 Amount = 1);
	UFUNCTION()
	void OnAddItemRequest(TSubclassOf<APickupItem> ItemClass);
	UFUNCTION()
	void RemoveItem(TSubclassOf<APickupItem> ItemClass, int32 Amount = 1);
	UFUNCTION()
	void OnRemoveItemRequest(TSubclassOf<APickupItem> Item);
	
	bool HasItemClass(TSubclassOf<APickupItem> ItemClass) const;
	int32 GetItemAmount(TSubclassOf<APickupItem> ItemClass) const;

protected:
	virtual void BeginPlay() override;
	
public:
	// Bag class
	UPROPERTY(EditAnywhere, Category= "Bag")
	TSubclassOf<ABagActor> BagActorClass;
protected:
	TWeakObjectPtr<AWizardPlayer> BagOwner;
private:
	UPROPERTY()
	TMap<TSubclassOf<APickupItem>, int32> Items;

	// 3D UI Actor
	TWeakObjectPtr<ABagActor> BagActor;

	bool bPlayerCursorInBounds = false;
};
