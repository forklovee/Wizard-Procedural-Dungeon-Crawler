// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTileContextMenu.generated.h"

struct FInventorySlot;
class UTextBlock;
class UVerticalBox;
class UContextMenuActionButton;
class UItemTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemTileContextMenuOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemTileContextMenuClosed);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseActionButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDropActionButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInspectActionButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelActionButtonClicked);

UCLASS()
class PROCDUNGEONCRAWLER_API UItemTileContextMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnItemTileContextMenuOpened OnItemTileContextMenuOpened;
	UPROPERTY(BlueprintAssignable)
	FOnItemTileContextMenuClosed OnItemTileContextMenuClosed;

	UPROPERTY(BlueprintAssignable)
	FOnUseActionButtonClicked OnUseActionButtonClicked;
	UPROPERTY(BlueprintAssignable)
	FOnDropActionButtonClicked OnDropActionButtonClicked;
	UPROPERTY(BlueprintAssignable)
	FOnInspectActionButtonClicked OnInspectActionButtonClicked;
	UPROPERTY(BlueprintAssignable)
	FOnCancelActionButtonClicked OnCancelActionButtonClicked;

	UFUNCTION()
	void Open(UItemTile* ItemTile, FVector2D AbsolutePosition);
	UFUNCTION()
	void Close();

	UFUNCTION()
	void OnInventoryVisibilityChanged(bool bNewIsInventoryVisible);
protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnItemTileUpdated(UItemTile* ItemTile, FInventorySlot& InventorySlot);
	
	UFUNCTION()
	void OnUseActionButton();
	UFUNCTION()
	void OnDropActionButton();
	UFUNCTION()
	void OnInspectActionButton();
	UFUNCTION()
	void OnCloseActionButton();

private:
	void SetItemNameText(const FInventorySlot& InventorySlot) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWidget* ContextMenuRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemNameText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UContextMenuActionButton* UseButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UContextMenuActionButton* DropButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UContextMenuActionButton* InspectButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UContextMenuActionButton* CloseButton;

private:
	TWeakObjectPtr<UItemTile> TargetItemTile;
};
