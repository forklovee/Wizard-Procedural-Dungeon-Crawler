// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BagComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBagUpdated, TSet<TSubclassOf<AActor>>, Items);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UBagComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnBagUpdated OnBagUpdated;
	
	UBagComponent();

	TMap<TSubclassOf<AActor>, int32>& GetItems() { return Items; }
	
	void AddItem(TSubclassOf<AActor> ItemClass, int32 Amount = 1);
	void RemoveItem(TSubclassOf<AActor> ItemClass, int32 Amount = 1);
	
	bool HasItemClass(TSubclassOf<AActor> ItemClass) const;
	int32 GetItemAmount(TSubclassOf<AActor> ItemClass) const;

	TSet<TSubclassOf<AActor>> GetItemClasses() const;

private:
	UPROPERTY()
	TMap<TSubclassOf<AActor>, int32> Items;
		
};
