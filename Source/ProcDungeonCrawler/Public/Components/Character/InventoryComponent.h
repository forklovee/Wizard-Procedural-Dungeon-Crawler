// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Item.h"
#include "InventoryComponent.generated.h"

class AWeapon;
class AWizardPlayer;

USTRUCT()
struct FInvSlot
{
	GENERATED_BODY()

	FInvSlot() = default;
	
	FInvSlot(int NewIndex, FVector2D NewTilePos)
	{
		Index = NewIndex;
		TilePos = NewTilePos;
	}

	FInvSlot(TSubclassOf<AItem> NewItemClass, int32 NewAmount)
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
	TSubclassOf<AItem> ItemClass;
	int32 Amount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemAdded, FInvSlot, BagSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FInvSlot, BagSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvSlotUpdated, FInvSlot, BagSlot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnNewItemAdded OnNewItemAdded;
	FOnItemRemoved OnItemRemoved;
	FOnInvSlotUpdated OnInvSlotUpdated;
	
	UInventoryComponent();
	
	UFUNCTION()
	void AddItem(TSubclassOf<AItem> ItemClass, int32 Amount = 1);
	UFUNCTION()
	void RemoveItem(TSubclassOf<AItem> ItemClass, int32 Amount = 1);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateItemSlot(FInvSlot BagSlotOverride);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag", meta=(AllowPrivateAccess = "true"))
	FVector2D BagGridSize = FVector2D(4, 3);
	
	TWeakObjectPtr<AWizardPlayer> BagOwner;
private:
	TSubclassOf<AWeapon> WeaponClass;
	
	TArray<FInvSlot> Items;
};
