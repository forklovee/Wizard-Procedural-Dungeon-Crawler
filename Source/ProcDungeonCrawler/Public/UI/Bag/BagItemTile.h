// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "..\..\Components\Character\InventoryComponent.h"
#include "BagItemTile.generated.h"

struct FInvSlot;
class UTextBlock;
class UButton;
class AItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHoverChanged, UBagItemTile*, HoveredTile, bool, bIsHovered);

UCLASS()
class PROCDUNGEONCRAWLER_API UBagItemTile : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnHoverChanged OnHoverChanged;
	
	void UpdateVisualData();

	void SetCanBeTargeted(bool bIsTargetable);
	
	void SetBagSlot(FInvSlot* NewBagSlot);
	FInvSlot* GetBagSlot() const;
	
	void SetPickupItemActor(AItem* PickupItem);
	AItem* GetPickupItemActor() const;

	void SetItemAmountTextBlock(int32 ItemAmount);

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

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* DragItemTextBlock;	
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ItemAmountTextBlock;

	FInvSlot LastBagSlotData;
	bool bItemActorGrabChangeEventConnected = false;
protected:
	FSlateBrush DefaultButtonBrush;
	FSlateBrush HoverButtonBrush;
	
	FInvSlot* BagSlot = nullptr;
	TWeakObjectPtr<AItem> PickupItemActor;

	bool bCanBeTargeted = false;
	bool bIsTargeted = false;
};
