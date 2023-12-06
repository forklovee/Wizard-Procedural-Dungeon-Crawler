// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IntVectorTypes.h"
#include "Components/Character/BagComponent.h"
#include "GameFramework/Actor.h"
#include "BagActor.generated.h"

class UBagComponent;
class UBagItemTile;
class APickupItem;
class UBagUI;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabbedActorPositionOverrideRequest, FVector, NewPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbedActorPositionOverrideClearRequest);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItemRequest, FBagSlot, BagSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveItemRequest,  FBagSlot, BagSlot);

UCLASS()
class PROCDUNGEONCRAWLER_API ABagActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Interaction
	FOnGrabbedActorPositionOverrideRequest OnGrabbedActorPositionOverrideRequest;
	FOnGrabbedActorPositionOverrideClearRequest OnGrabbedActorPositionOverrideClearRequest;
	// Bag Component Item Changes
	FOnAddItemRequest OnAddItemRequest;
	FOnRemoveItemRequest OnRemoveItemRequest;
	
	ABagActor();

	// Bag Open/Close requests
	void Open(UBagComponent* BagComponent);
	void Close();

	// Player hand interactions
	UFUNCTION()
	void BeginItemGrab(UPrimitiveComponent* GrabComponent, AActor* Actor);
	UFUNCTION()
	void OnPlayerCursorHoverChanged(bool bIsHovered, AActor* PlayerGrabbedItem = nullptr);
	UFUNCTION()
	void CommitGrabbedItemAction(UPrimitiveComponent* GrabComponent, AActor* Actor);
	
	bool IsOpen() const { return bIsOpen; }

	// Item actors management
	UFUNCTION()
	void SpawnItemActor(UBagItemTile* ItemTile);
	UFUNCTION()
	void DestroyItemActor(UBagItemTile* ItemTile);

	// On Bag Component Item Changes
	UFUNCTION()
	void OnNewItemAdded(FBagSlot Slot);
	UFUNCTION()
	void OnItemRemoved(FBagSlot Slot);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBagUI> BagUIClass;
	
	FVector GetSlotLocation(const FBagSlot* BagSlot) const;

private:
	UBagItemTile* GetBagItemTileByItemClass(TSubclassOf<APickupItem> ItemClass);

	UFUNCTION()
	void OnTargetTileChanged(UBagItemTile* HoveredTile, bool bIsHovered);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BagMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* BagWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* SlotsRootComponent;
	
	bool bItemTileActorManagementRegistered = false;
private:
	bool bIsOpen = false;

	TWeakObjectPtr<UBagComponent> Bag;
	TWeakObjectPtr<UBagUI> BagUI;
	TArray<UBagItemTile*> BagItemTiles;
	
	FVector2D GridSize;

	TWeakObjectPtr<APickupItem> GrabbedItemActor;
	FBagSlot GrabbedItemData;
	
	TWeakObjectPtr<UBagItemTile> HoveredItemTile;
	TWeakObjectPtr<UBagItemTile> LastHoveredItemTile;
	FBagSlot LastItemTileBagSlot;
};
