// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Character/InventoryComponent.h"
#include "Items/Item.h"
#include "ItemTile.generated.h"

class UOverlay;
class UButton;
struct FInventorySlot;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemTileMouseHovered, UItemTile*, ItemTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemTileMouseUnHovered, UItemTile*, ItemTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnContextMenuRequested, UItemTile*, ItemTile, FVector2D, ViewportPosition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseItemRequest, TSubclassOf<AItem>, ItemClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInspectItemRequest, TSubclassOf<AItem>, ItemClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDropItemRequest, TSubclassOf<AItem>, ItemClass, int32, Amount);

UCLASS()
class PROCDUNGEONCRAWLER_API UItemTile : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnItemTileMouseHovered OnItemTileMouseHovered;
	UPROPERTY(BlueprintAssignable)
	FOnItemTileMouseUnHovered OnItemTileMouseUnHovered;
	
	UPROPERTY(BlueprintAssignable)
	FOnItemClicked OnItemClicked;
	UPROPERTY(BlueprintAssignable)
	FOnContextMenuRequested OnContextMenuRequested;

	UPROPERTY(BlueprintAssignable)
	FOnUseItemRequest OnUseItemRequest;
	UPROPERTY(BlueprintAssignable)
	FOnInspectItemRequest OnInspectItemRequest;
	UPROPERTY(BlueprintAssignable)
	FOnDropItemRequest OnDropItemRequest;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	void UpdateData(FInventorySlot InventorySlot);

protected:
	UFUNCTION()
	void UseItem();
	void InspectItem();
	void DropItem();

private:
	void ResetUseItemTapCounter();

	UFUNCTION()
	void OnItemTileHovered();
	UFUNCTION()
	void OnItemTileUnHovered();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* ItemTileMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AmountTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* ContextMenu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* UseItemButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* DropItemButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UButton* CloseContextMenuButton;
private:
	FInventorySlot ItemData;
	FVector2D TilePos;

	FTimerHandle UseItemDoubleTapTimerHandle;
	int UseItemTapCount = 0;
};
