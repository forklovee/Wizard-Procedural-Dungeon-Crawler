// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonConfig.generated.h"

class UDataTable;
class ADungeonObstacle;
class UDungeonRuleDictionary;
class UDungeonRoomDictionary;

USTRUCT(BlueprintType)
struct FMaxLootOnLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxWeapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRunes;
};

UCLASS()
class PROCDUNGEONCRAWLER_API UDungeonConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Levels")
	int32 DungeonLevelsAmount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	TSoftObjectPtr<UDungeonRuleDictionary> DungeonRuleDictionaryDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rooms")
	TSoftObjectPtr<UDungeonRoomDictionary> DungeonRoomDictionaryDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rooms")
	TMap<int32, int32> MaxRoomAmountOnFloor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
	TSoftObjectPtr<UDataTable> DungeonAssetDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Obstacles")
	TSoftObjectPtr<UDataTable> ObstacleDataTable;
};
