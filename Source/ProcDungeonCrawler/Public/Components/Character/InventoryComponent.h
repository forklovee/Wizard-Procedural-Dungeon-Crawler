// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Item.h"
#include "InventoryComponent.generated.h"

class AWeapon;
class AWizardPlayer;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	FInventorySlot()
	{
		ItemClass = nullptr;
		Amount = 0;
	}

	FInventorySlot(TSubclassOf<AItem> NewItemClass, int32 NewAmount)
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
	
	FVector2D TilePos;
	TSubclassOf<AItem> ItemClass;
	int32 Amount = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, int, SlotIndex, FInventorySlot, InventorySlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, int, SlotIndex, FInventorySlot, InventorySlot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnItemAdded OnItemAdded;
	FOnItemRemoved OnItemRemoved;
	
	UInventoryComponent();
	
	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<AItem> ItemClass, int32 Amount = 1);
	UFUNCTION(BlueprintCallable)
	void RemoveItem(TSubclassOf<AItem> ItemClass, int32 Amount = 1);
	
	UFUNCTION(BlueprintCallable)
	bool HasItem(TSubclassOf<AItem> ItemClass);
	UFUNCTION(BlueprintCallable)
	int GetItemAmount(TSubclassOf<AItem> ItemClass);

	UFUNCTION(BlueprintCallable)
	bool IsTileFree(FVector2D TilePos);

	FVector2D GetInventorySize() const { return InventorySize; }
protected:
	virtual void BeginPlay() override;

	FVector2D GetFirstFreeTile() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag", meta=(AllowPrivateAccess = "true"))
	FVector2D InventorySize = FVector2D(6, 15);
	
private:
	TArray<FInventorySlot> ItemSlots;
};
