// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IntVectorTypes.h"
#include "Components/ActorComponent.h"
#include "Items/PickupItem.h"
#include "BagComponent.generated.h"

class AWeapon;
class AWizardPlayer;
class UWidgetComponent;
class APickupItem;
class ABagActor;
struct FInputActionValue;

USTRUCT()
struct FBagSlot
{
	GENERATED_BODY()

	FBagSlot() = default;
	
	FBagSlot(int NewIndex, FVector2D NewTilePos)
	{
		Index = NewIndex;
		TilePos = NewTilePos;
	}

	FBagSlot(TSubclassOf<APickupItem> NewItemClass, int32 NewAmount)
	{
		ItemClass = NewItemClass;
		Amount = NewAmount;
	}

	void Clear()
	{
		ItemClass = nullptr;
		Amount = 0;
	}

	bool IsEmpty() const
	{
		return ItemClass == nullptr;
	}

	int Index = -1;
	FVector2D TilePos;
	TSubclassOf<APickupItem> ItemClass;
	int32 Amount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBagContentsUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBagStateChanged, bool, bIsOpen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerCursorHoverChanged, bool, bIsHovered, AActor*, PlayerGrabbedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLeftRightInput, int, Direction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemAdded, FBagSlot, BagSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FBagSlot, BagSlot);
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
	
	UFUNCTION()
	void OnLeftRightInputAction(int Direction);

	TArray<FBagSlot>& GetItems() { return Items; }
	int GetBagSize() const;
	FVector2D GetBagGridSize() const;
	int GetFreeTileAmount();
	bool IsBagFull();
	
	FBagSlot* GetItemByPosition(const FVector2D& TilePos);
	FBagSlot* GetItemByClass(const TSubclassOf<APickupItem> ItemClass);

	bool HasItem(const TSubclassOf<APickupItem> ItemClass);

	FBagSlot* GetFirstFreeTile();
	
	UFUNCTION()
	void AddItem(TSubclassOf<APickupItem> ItemClass, int32 Amount = 1);
	UFUNCTION()
	void RemoveItem(TSubclassOf<APickupItem> ItemClass, int32 Amount = 1);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateItemSlot(FBagSlot BagSlotOverride);
public:
	// Bag class
	UPROPERTY(EditAnywhere, Category= "Bag")
	TSubclassOf<ABagActor> BagActorClass;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag", meta=(AllowPrivateAccess = "true"))
	FVector2D BagGridSize = FVector2D(4, 3);
	
	TWeakObjectPtr<AWizardPlayer> BagOwner;
private:
	TSubclassOf<AWeapon> WeaponClass;
	TWeakObjectPtr<AWeapon> WeaponInHand;
	
	TArray<FBagSlot> Items;

	// 3D UI Actor
	TWeakObjectPtr<ABagActor> BagActor;

	bool bPlayerCursorInBounds = false;
};
