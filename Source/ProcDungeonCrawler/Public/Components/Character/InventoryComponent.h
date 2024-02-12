// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Items/Clothes/Clothes.h"
#include "InventoryComponent.generated.h"

enum class EArmorTarget : uint8;
class AClothes;
class APickupItem;
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

	FInventorySlot(const uint8 NewIndex, const int Row, const int Column)
	{
		Index = NewIndex;
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

	static FInventorySlot EmptySlot;
	
	uint8 Index;
	FText ItemNameText;
	TSubclassOf<AItem> ItemClass;
	TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;
	bool bIsConsumable = false;
	
	FVector2D TilePos;
	uint8 MaxStackSize = 1;
	uint8 Amount = 0;

	TWeakObjectPtr<AActor> SpawnedActor;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, FInventorySlot&, InventorySlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FInventorySlot&, InventorySlot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquipped, AWeapon*, WeaponActor, FInventorySlot&, InventorySlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponUnEquipped, AWeapon*, WeaponActor, FInventorySlot&, InventorySlot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClothEquipped, AClothes*, ClothActor, FInventorySlot&, InventorySlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClothUnEquipped, AClothes*, ClothActor, FInventorySlot&, InventorySlot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnItemAdded OnItemAdded;
	FOnItemRemoved OnItemRemoved;

	FOnWeaponEquipped OnWeaponEquipped;
	FOnWeaponUnEquipped OnWeaponUnEquipped;

	FOnClothEquipped OnClothEquipped;
	FOnClothUnEquipped OnClothUnEquipped;
	
	UInventoryComponent();

	// Equipment
	UFUNCTION(BlueprintCallable)
	bool HasEquippedWeapon() const { return WeaponSlot != nullptr; }
	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon() const { return WeaponSlot != nullptr ? Cast<AWeapon>(WeaponSlot->SpawnedActor.Get()) : nullptr; }
	
	UFUNCTION(BlueprintCallable)
	bool HasEquippedArmor(EArmorTarget ArmorTarget) const { return EquippedArmorSlots.Contains(ArmorTarget); }

	// Inventory
	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlot>& GetItemSlots();
	
	UFUNCTION(BlueprintCallable)
	void UseItem(FInventorySlot& InventorySlot);

	UFUNCTION(BlueprintCallable)
	void DropItem(FInventorySlot& InventorySlot);
	
	UFUNCTION(BlueprintCallable)
	FInventorySlot GetItemSlotByIndex(const uint8 SlotIdx);
	UFUNCTION(BlueprintCallable)
	FInventorySlot GetItemSlotByGridPosition(const FVector2D TilePos);

	UFUNCTION(BlueprintCallable)
	bool HasItemOfClass(TSubclassOf<AItem> ItemClass, int32& OutAmount) const;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(AItem* ItemActor, int32 Amount = 1);
	
	UFUNCTION(BlueprintCallable)
	void RemoveItemByClass(TSubclassOf<AItem> ItemClass, int32 Amount = 1);
	UFUNCTION(BlueprintCallable)
	void RemoveItemBySlotIndex(uint8 SlotIdx, int32 Amount = 1);

	UFUNCTION(BlueprintCallable)
	void MarkInventorySlotAsEquipped(APickupItem* EquippedActor, FInventorySlot& InventorySlot);

	FVector2D GetInventorySize() const { return InventorySize; }
	
protected:
	virtual void BeginPlay() override;
	
	void EquipItem(FInventorySlot& InventorySlot);
	void UnEquipItem(FInventorySlot& InventorySlot);
	
	void RemoveItem(FInventorySlot& InventorySlot, int32 Amount);
	
	int GetFirstFreeInventorySlotId() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bag", meta=(AllowPrivateAccess = "true"))
	FVector2D InventorySize = FVector2D(6, 15);
	
private:
	TArray<FInventorySlot> ItemSlots;

	FInventorySlot* WeaponSlot;
	TMap<EArmorTarget, FInventorySlot*> EquippedArmorSlots;
};
