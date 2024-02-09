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

	FInventorySlot(const int Row, const int Column)
	{
		ItemClass = nullptr;
		Amount = 0;
		TilePos = FVector2D(Row, Column);
	}
	
	FInventorySlot(const FText NewItemNameText, const TSubclassOf<AItem> NewItemClass, const UTexture2D* NewItemIcon, const int32 NewAmount)
	{
		ItemNameText = NewItemNameText;
		ItemClass = NewItemClass;
		ItemIcon = NewItemIcon;
		Amount = NewAmount;
	}

	FORCEINLINE bool operator == (const FInventorySlot& Other) const
	{
		return ItemClass == Other.ItemClass;
	}
	
	void Clear()
	{
		ItemClass = nullptr;
		ItemIcon = nullptr;
		Amount = 0;
	}

	bool IsEmpty() const
	{
		return Amount <= 0;
	}

	FText ItemNameText;
	TSubclassOf<AItem> ItemClass;
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;
	bool bIsConsumable = false;
	
	FVector2D TilePos;
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
	void AddItem(AItem* ItemActor, int32 Amount = 1);
	
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

	int GetFirstFreeInventorySlotId() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag", meta=(AllowPrivateAccess = "true"))
	FVector2D InventorySize = FVector2D(6, 15);
	
private:
	TArray<FInventorySlot> ItemSlots;
};
