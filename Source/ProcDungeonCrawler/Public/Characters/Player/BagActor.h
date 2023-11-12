// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BagActor.generated.h"

class UBagItemTile;
class APickupItem;
class UBagUI;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestRemoveItemFromBag, int, SlotIdx, APickupItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestAddItemFromBag, int, SlotIdx, APickupItem*, Item);

UCLASS()
class PROCDUNGEONCRAWLER_API ABagActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABagActor();

	UFUNCTION()
	void OnPlayerCursorHoverChanged(bool bIsHovered);
	
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION()
	void SetupView();

	UFUNCTION()
	void ClearView();
	
	UFUNCTION()
	void ChangeSlotsPage(int PageOffset);
	
	UFUNCTION()
	void SetPawnItems(TMap<TSubclassOf<APickupItem>, int32>& Items);

	UFUNCTION()
	void SpawnItemActor(UBagItemTile* ItemTile);

	UFUNCTION()
	void DestroyItemActor(UBagItemTile* ItemTile);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void SetGrabbedItemTile(UBagItemTile* Item, bool bIsGrabbed);
	FVector GetSlotLocation(int SlotIdx) const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBagUI> BagUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BagMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* BagWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* SlotsRootComponent;
	
private:
	UPROPERTY(EditAnywhere, Category="Slots")
	int ViewSlots = 3;
	
	UPROPERTY(EditAnywhere, Category="Slots")
	float SlotOffset = 100.f;
	
	bool bIsOpen = false;

	int ViewStartIdx = 0;
	int LastViewStartIdx = 0;
	
	TMap<TSubclassOf<APickupItem>, int32>* PawnItems;
	TWeakObjectPtr<UBagItemTile> GrabbedItemTile;
};
