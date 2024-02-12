// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Character/InventoryComponent.h"

#include "Characters/Human/Human.h"
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Items/Clothes/Clothes.h"
#include "Items/Consumables/ConsumableItem.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquippedArmorSlots.Add(EArmorTarget::Head);
	EquippedArmorSlots.Add(EArmorTarget::Chest);
	EquippedArmorSlots.Add(EArmorTarget::Feet);
	EquippedArmorSlots.Add(EArmorTarget::Hands);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < InventorySize.X; i++)
	{
		for (int j = 0; j < InventorySize.Y; j++)
		{
			ItemSlots.Add(FInventorySlot(ItemSlots.Num(), i, j));
		}
	}
}

TArray<FInventorySlot>& UInventoryComponent::GetItemSlots()
{
	return ItemSlots;
}

void UInventoryComponent::UseItem(FInventorySlot& InventorySlot)
{
	if (InventorySlot.ItemClass == nullptr) return;

	// Check if inventory slot item is equipped
	// If it is, unequip it
	if (InventorySlot.SpawnedActor != nullptr)
	{
		UnEquipItem(InventorySlot);
		return;
	}
	
	AActor* Actor = GetWorld()->SpawnActor(InventorySlot.ItemClass);
	if (AConsumableItem* Consumable = Cast<AConsumableItem>(Actor))
	{
		Consumable->Use(Cast<AHuman>(GetOwner()));
		RemoveItem(InventorySlot, 1);
	}
	else if (AClothes* Clothes = Cast<AClothes>(Actor))
	{
		// Equip clothes
		InventorySlot.SpawnedActor = Actor;
		EquipItem(InventorySlot);
		return;
	}
	else if (AWeapon* Weapon = Cast<AWeapon>(Actor))
	{
		// Equip weapon
		InventorySlot.SpawnedActor = Actor;
		EquipItem(InventorySlot);
		return;
	}
	
	Actor->Destroy();
}

void UInventoryComponent::DropItem(FInventorySlot& InventorySlot)
{
	if (InventorySlot.ItemClass == nullptr) return;
	
	// If it is equipped, unequip it
	if (InventorySlot.SpawnedActor != nullptr && InventorySlot.Amount == 1)
	{
		UnEquipItem(InventorySlot);
	}

	const FVector SpawnLocation = GetOwner()->GetActorLocation();
	AActor* Actor = GetWorld()->SpawnActor(InventorySlot.ItemClass, &SpawnLocation);
	
	RemoveItem(InventorySlot, 1);
}

FInventorySlot UInventoryComponent::GetItemSlotByIndex(const uint8 SlotIdx)
{
	return ItemSlots[SlotIdx];
}

FInventorySlot UInventoryComponent::GetItemSlotByGridPosition(const FVector2D TilePos)
{
	for (FInventorySlot& Slot : ItemSlots)
	{
		if (Slot.TilePos == TilePos)
		{
			return Slot;
		}
	}
	return FInventorySlot();
}

bool UInventoryComponent::HasItemOfClass(TSubclassOf<AItem> ItemClass, int32& OutAmount) const
{
	const int32 ItemSlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});
	if (ItemSlotIndex != -1)
	{
		OutAmount = ItemSlots[ItemSlotIndex].Amount;
		return true;
	}
	return false;
}

void UInventoryComponent::EquipItem(FInventorySlot& InventorySlot)
{
	UE_LOG(LogTemp, Display, TEXT("Equip! %i %i"), InventorySlot.ItemClass == nullptr, InventorySlot.SpawnedActor == nullptr)
	if (InventorySlot.ItemClass == nullptr || InventorySlot.SpawnedActor == nullptr) return;
	
	if (AWeapon* Weapon = Cast<AWeapon>(InventorySlot.SpawnedActor))
	{
		if (WeaponSlot != nullptr)
		{
			UnEquipItem(*WeaponSlot);
		}
		
		WeaponSlot = &InventorySlot;
		if (OnWeaponEquipped.IsBound())
		{
			OnWeaponEquipped.Broadcast(Weapon, InventorySlot);
		}
	}
	else if (AClothes* Clothes = Cast<AClothes>(InventorySlot.SpawnedActor))
	{
		EquippedArmorSlots[Clothes->ArmorTarget] = &InventorySlot;
		if (OnClothEquipped.IsBound())
		{
			OnClothEquipped.Broadcast(Clothes, InventorySlot);
		}
	}
}

void UInventoryComponent::UnEquipItem(FInventorySlot& InventorySlot)
{
	if (InventorySlot.ItemClass == nullptr || InventorySlot.SpawnedActor == nullptr) return;
	
	if (AWeapon* Weapon = Cast<AWeapon>(InventorySlot.SpawnedActor))
	{
		WeaponSlot = nullptr;
		if (OnWeaponUnEquipped.IsBound())
		{
			OnWeaponUnEquipped.Broadcast(nullptr, InventorySlot);
		}
	}
	else if (AClothes* Clothes = Cast<AClothes>(InventorySlot.SpawnedActor))
	{
		EquippedArmorSlots[Clothes->ArmorTarget] = nullptr;
		if (OnClothEquipped.IsBound())
		{
			OnClothUnEquipped.Broadcast(nullptr, InventorySlot);
		}
	}
	
	InventorySlot.SpawnedActor->Destroy();
}

int UInventoryComponent::GetFirstFreeInventorySlotId() const
{
	return ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.IsEmpty();
	});
}

void UInventoryComponent::AddItem(AItem* ItemActor, int32 Amount)
{
	if (!IsValid(ItemActor))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Item actor."))
		return;
	}
	
	const TSubclassOf<AItem> ItemClass = ItemActor->GetClass();
	int SlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass && Slot.Amount + Amount <= Slot.MaxStackSize;
	});
	
	if (SlotIndex != -1) // Item exists, add amount
	{
		ItemSlots[SlotIndex].Amount += Amount;

		if (OnItemAdded.IsBound())
		{
			OnItemAdded.Broadcast(ItemSlots[SlotIndex]);
		}
		ItemActor->Destroy();
		return;
	}
	// Item doesn't exist, add new item
	// Check if there is a free tile
	SlotIndex = GetFirstFreeInventorySlotId();
	if (SlotIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory full"))
		return;
	}

	ItemSlots[SlotIndex].ItemNameText = ItemActor->GetItemName();
	ItemSlots[SlotIndex].ItemClass = ItemClass;
	
	ItemSlots[SlotIndex].MaxStackSize = ItemActor->GetMaxStackSize();
	ItemSlots[SlotIndex].Amount = Amount;
	
	ItemSlots[SlotIndex].ItemIcon = ItemActor->GetItemIcon();
	
	UE_LOG(LogTemp, Display, TEXT("Item: %s - Amount: %d"), *ItemClass->GetName(), ItemSlots[SlotIndex].Amount)
	
	if (OnItemAdded.IsBound())
	{
		OnItemAdded.Broadcast(ItemSlots[SlotIndex]);
	}

	ItemActor->Destroy();
}

void UInventoryComponent::RemoveItemByClass(TSubclassOf<AItem> ItemClass, int32 Amount)
{
	const int SlotIndex = ItemSlots.IndexOfByPredicate([=](const FInventorySlot& Slot) {
		return Slot.ItemClass == ItemClass;
	});
	
	if (SlotIndex == -1)
	{
		UE_LOG(LogTemp, Display, TEXT("Item not found."))
		return;
	}

	RemoveItem(ItemSlots[SlotIndex], Amount);
}

void UInventoryComponent::RemoveItemBySlotIndex(uint8 SlotIdx, int32 Amount)
{
	if (ItemSlots.Num() >= SlotIdx) return;

	RemoveItem(ItemSlots[SlotIdx], Amount);
}

void UInventoryComponent::MarkInventorySlotAsEquipped(APickupItem* EquippedActor, FInventorySlot& InventorySlot)
{
	if (InventorySlot.ItemClass == nullptr || InventorySlot.SpawnedActor.Get() != EquippedActor) return;
}

void UInventoryComponent::RemoveItem(FInventorySlot& InventorySlot, int32 Amount)
{
	InventorySlot.Amount -= Amount;
	if (InventorySlot.Amount <= 0)
	{
		InventorySlot.Clear();
	}

	if (OnItemRemoved.IsBound())
	{
		OnItemRemoved.Broadcast(InventorySlot);
	}
}
