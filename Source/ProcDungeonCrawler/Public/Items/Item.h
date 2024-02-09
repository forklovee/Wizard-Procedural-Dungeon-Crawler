// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class AHuman;
class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickedUp);

UCLASS()
class AItem : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnPickedUp OnPickedUp;
	
	AItem();

	UFUNCTION(BlueprintCallable, Category = "Item|Usage")
	virtual void Use(AHuman* HumanPawn);
	
	UFUNCTION(BlueprintCallable, Category = "Item|Equip")
	virtual void Equip(AHuman* HumanPawn, USceneComponent* EquipTargetComponent, FName SocketName);
	UFUNCTION(BlueprintCallable, Category = "Item|Equip")
	virtual void UnEquip();
	
	UFUNCTION(BlueprintCallable, Category = "Item|Pickup")
	virtual TSubclassOf<AActor> Pickup(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category = "Item|Visual")
	UTexture2D* GetItemIcon() const { return ItemIcon; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	const FText& GetItemName() const { return ItemName; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	FText ItemName = FText::FromString("Item");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	UTexture2D* ItemIcon = nullptr;
	
	FTimerHandle DestroyTimerHandle;
};
