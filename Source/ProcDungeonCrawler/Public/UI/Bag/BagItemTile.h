// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Character/BagComponent.h"
#include "BagItemTile.generated.h"

struct FBagSlot;
class UTextBlock;
class UButton;
class APickupItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHoverChanged, UBagItemTile*, HoveredTile, bool, bIsHovered);

UCLASS()
class PROCDUNGEONCRAWLER_API UBagItemTile : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnHoverChanged OnHoverChanged;
	
	void UpdateVisualData();

	void SetCanBeTargeted(bool bIsTargetable);
	
	void SetBagSlot(FBagSlot* NewBagSlot);
	FBagSlot* GetBagSlot() const;
	
	void SetPickupItemActor(APickupItem* PickupItem);
	APickupItem* GetPickupItemActor() const;

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

	FBagSlot LastBagSlotData;
	bool bItemActorGrabChangeEventConnected = false;
protected:
	FSlateBrush DefaultButtonBrush;
	FSlateBrush HoverButtonBrush;
	
	FBagSlot* BagSlot = nullptr;
	TWeakObjectPtr<APickupItem> PickupItemActor;

	bool bCanBeTargeted = false;
	bool bIsTargeted = false;
};
