// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BagUI.generated.h"

class UBagItemTile;
class ABagActor;
class APickupItem;
class UScrollBox;
class UUniformGridPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemTileCreated, UBagItemTile*, ItemTile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemTileRemoved, UBagItemTile*, ItemTile);

UCLASS()
class PROCDUNGEONCRAWLER_API UBagUI : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnNewItemTileCreated OnNewItemTileCreated;
	FOnNewItemTileRemoved OnNewItemTileRemoved;
	
	TMap<TSubclassOf<APickupItem>, UBagItemTile*> SetupBagUI(ABagActor* BagActor, TMap<TSubclassOf<APickupItem>, int32>* PawnItemsPtr);
	
	
	TArray<APickupItem*> GetAllSpawnedActors() const;
	
	UBagItemTile* CreateNewItemTile(TSubclassOf<APickupItem> ItemClass, int32 Amount);
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BagItemWidgetClass;
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UPanelWidget* BagItemsPanel;

private:
	TArray<UBagItemTile*> BagItemTiles;
};
