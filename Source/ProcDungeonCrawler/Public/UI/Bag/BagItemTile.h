// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BagItemTile.generated.h"

class UButton;
class APickupItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnPickupItemRequest, UBagItemTile*, BagItemTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestroyPickupItemRequest, UBagItemTile*, BagItemTile);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemActorGrabbedChanged, UBagItemTile*, BagItemTile, bool, bIsGrabbed);

UCLASS()
class PROCDUNGEONCRAWLER_API UBagItemTile : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnSpawnPickupItemRequest OnSpawnPickupItemRequest;
	FOnDestroyPickupItemRequest OnDestroyPickupItemRequest;
	FOnItemActorGrabbedChanged OnItemActorGrabbedChanged;
	
	void SpawnPickupItem();
	void DestroyPickupItem();
	
	void SetPickupItemClass(TSubclassOf<APickupItem> PickupItem);
	TSubclassOf<APickupItem> GetPickupItemClass() const;

	void SetPickupItemActor(APickupItem* PickupItem);
	APickupItem* GetPickupItemActor() const;
	
	int32 GetIndex() const;
	
	UFUNCTION()
	bool IsTargeted() const;
	
	UFUNCTION()
	void OnItemGrabbed(APickupItem* Item, bool bIsGrabbed);

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnTileHovered();
	UFUNCTION()
	void OnTileUnHovered();
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* TileButton;

	bool bItemActorGrabChangeEventConnected = false;
protected:
	TSubclassOf<APickupItem> PickupItemClass;
	TWeakObjectPtr<APickupItem> PickupItemActor;
	
	bool bIsTargeted = false;
};
