// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BagUI.generated.h"

class UBagComponent;
struct FBagSlot;
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
	
	TArray<UBagItemTile*>& Setup(ABagActor* BagActor, UBagComponent* BagComponent);
	
	TArray<APickupItem*> GetAllSpawnedActors() const;

private:
	UBagItemTile* CreateNewBagTile(FBagSlot& BagSlot);

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BagItemWidgetClass;
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UUniformGridPanel* BagItemsPanel;

private:
	TArray<UBagItemTile*> BagItemTiles;
	FVector2D GridSize = FVector2D(3, 3);
};
