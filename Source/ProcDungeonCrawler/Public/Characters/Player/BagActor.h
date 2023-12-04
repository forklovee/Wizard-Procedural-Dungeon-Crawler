// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BagActor.generated.h"

class UBagItemTile;
class APickupItem;
class UBagUI;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrabbedActorPositionOverrideRequest, FVector, NewPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrabbedActorPositionOverrideClearRequest);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItemRequest, TSubclassOf<APickupItem>, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveItemRequest,  TSubclassOf<APickupItem>, Item);

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

	UFUNCTION()
	void BeginItemGrab(UPrimitiveComponent* GrabComponent, AActor* Actor);
	UFUNCTION()
	void OnPlayerCursorHoverChanged(bool bIsHovered, AActor* PlayerGrabbedItem = nullptr);
	UFUNCTION()
	void CommitGrabbedItemAction(UPrimitiveComponent* GrabComponent, AActor* Actor);
	
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION()
	void SetupView();
	UFUNCTION()
	void ClearView();
	
	UFUNCTION()
	void SetPawnItems(TMap<TSubclassOf<APickupItem>, int32>& Items);

	UFUNCTION()
	void SpawnItemActor(UBagItemTile* ItemTile);
	UFUNCTION()
	void DestroyItemActor(UBagItemTile* ItemTile);

	UFUNCTION()
	void OnNewItemAdded(TSubclassOf<APickupItem> ItemClass, int32 NewAmount);
	UFUNCTION()
	void OnItemRemoved(TSubclassOf<APickupItem> ItemClass, int32 NewAmount);
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBagUI> BagUIClass;
	
	FVector GetSlotLocation(int SlotIdx) const;

private:
	UFUNCTION()
	void ItemTile_SpawnAndDestroyRequests(UBagItemTile* ItemTile);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BagMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* BagWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* SlotsRootComponent;
	
	UPROPERTY(EditAnywhere, Category="Slots")
	FVector2D GridSize = FVector2D(4, 5);
	
	UPROPERTY(EditAnywhere, Category="Slots")
	float SlotOffset = 25.f;

private:
	TWeakObjectPtr<UBagUI> BagUI;
	bool bIsOpen = false;

	int ViewStartIdx = 0;
	int LastViewStartIdx = 0;
	
	TMap<TSubclassOf<APickupItem>, int32>* PawnItems;
	TMap<TSubclassOf<APickupItem>, UBagItemTile*> BagItemTiles;

	TWeakObjectPtr<AActor> GrabbedItemActor;
	bool bGrabbedItemHoverActionChanged = false;
	bool bRemoveGrabbedItemInitial = false;
	bool bRemoveGrabbedItem = false;
	
	TWeakObjectPtr<UBagItemTile> GrabbedItemTile;
};
