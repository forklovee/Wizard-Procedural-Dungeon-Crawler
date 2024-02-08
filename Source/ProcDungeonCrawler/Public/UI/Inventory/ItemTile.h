// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Character/InventoryComponent.h"
#include "Items/Item.h"
#include "ItemTile.generated.h"

class UButton;
struct FInventorySlot;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemAltClicked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseItemRequest, TSubclassOf<AItem>, ItemClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInspectItemRequest, TSubclassOf<AItem>, ItemClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDropItemRequest, TSubclassOf<AItem>, ItemClass, int32, Amount);

UCLASS()
class PROCDUNGEONCRAWLER_API UItemTile : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnItemClicked OnItemClicked;
	UPROPERTY(BlueprintAssignable)
	FOnItemAltClicked OnItemAltClicked;

	UPROPERTY(BlueprintAssignable)
	FOnUseItemRequest OnUseItemRequest;
	UPROPERTY(BlueprintAssignable)
	FOnInspectItemRequest OnInspectItemRequest;
	UPROPERTY(BlueprintAssignable)
	FOnDropItemRequest OnDropItemRequest;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	
	void UpdateData(FInventorySlot InventorySlot);

protected:
	UFUNCTION()
	void UseItem();
	void InspectItem();
	void DropItem();

private:
	void ResetUseItemTapCounter();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* ItemTileMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmountTextBlock;

private:
	FInventorySlot ItemData;
	FVector2D TilePos;

	FTimerHandle UseItemDoubleTapTimerHandle;
	int UseItemTapCount = 0;
};
