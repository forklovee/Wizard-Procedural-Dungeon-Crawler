// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonRuleDictionary.generated.h"

UENUM(BlueprintType)
enum class ERoomType: uint8
{
	None,
	
	Start,
	Hallway,
	Corridor,
	End,

	Basic,
	Basic_NoEnemies,
	
	Obstacle,
	Obstacle_NoEnemies,
	ObstacleSolver,
	ObstacleSolver_NoEnemies,
	
	Treasure,
	Treasure_NoEnemies,
	BigTreasure,
	BigTreasure_NoEnemies,
	
	Rune,
	Rune_NoEnemies,

	MiniBoss,
	MainBoss,
	
	Wildcard
};

USTRUCT(BlueprintType)
struct FRuleCollection
{
	GENERATED_BODY()
	FRuleCollection()
	{
		RoomType = ERoomType::None;
		Rules = {};
	}

	bool IsValid() const
	{
		return RoomType != ERoomType::None && Rules.Num() > 0;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERoomType RoomType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ERoomType> Rules;
};

UCLASS()
class PROCDUNGEONCRAWLER_API UDungeonRuleDictionary : public UDataAsset
{
	GENERATED_BODY()

public:
	bool HasRoomCollectionOfType(const ERoomType RoomType) const;
	
	FRuleCollection GetRandomRoomCollectionByType(const ERoomType RoomType) const;

private:
	TArray<FRuleCollection> GetRoomCollectionsByType(const ERoomType RoomType) const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	TArray<FRuleCollection> RuleCollections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wildcards")
	TArray<ERoomType> Wildcards;
};
