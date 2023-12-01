// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "World/DungeonGenerator/Rooms/RoomType.h"
#include "World/DungeonGenerator/DataAssets/DungeonAssets.h"
#include "DungeonRuleDictionary.generated.h"

enum class ERoomType: uint8;
class URoomType;

USTRUCT(BlueprintType)
struct FRuleProperties
{
	GENERATED_BODY()

	FRuleProperties()
	{
		RoomType = ERoomType::None;
		bHasObstacle = false;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERoomType RoomType;

	// Room Extension
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bMarkForExtension = true;
	
	// Obstacles
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasObstacle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bHasObstacle"))
	bool bUsePreferredObstacleClass = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bHasObstacle && bUsePreferredObstacleClass"))
	TSubclassOf<AActor> PreferredObstacleClass = nullptr;

	// Obstacle Solvers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bHasObstacle"))
	bool bCanHaveObstacleSolver = false;
	
	// Room Assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRoomPossibleAssetType> PossibleAssetTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRoomPossibleEnemyType> PossibleEnemyTypes;
};

USTRUCT(BlueprintType)
struct FRuleCollection
{
	GENERATED_BODY()
	FRuleCollection()
	{
		Rules = {};
	}

	bool IsValid() const
	{
		return Rules.Num() > 0;
	}

	int Num() const
	{
		return Rules.Num();
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="RoomType != ERoomType::None"))
	TArray<FRuleProperties> Rules;
};

USTRUCT(BlueprintType)
struct FFloorRoomStructure
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRuleCollection Structure;
};

USTRUCT(BlueprintType)
struct FRuleExtension
{
	GENERATED_BODY()
	FRuleExtension()
	{
		RoomType = ERoomType::None;
		Rules = {};
	}

	bool IsValid() const
	{
		return RoomType != ERoomType::None && Rules.Num() > 0;
	}
	
	int Num() const
	{
		return Rules.Num();
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERoomType RoomType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="RoomType != ERoomType::None"))
	FRuleCollection Rules;
};

UCLASS()
class PROCDUNGEONCRAWLER_API UDungeonRuleDictionary : public UDataAsset
{
	GENERATED_BODY()

public:
	int GetFloorAmount() const;
	
	bool HasFloorRoomStructures(const int Floor) const;
	FRuleCollection& GetFloorRoomStructure(const int Floor);
	
	bool HasRoomExtensionOfType(const ERoomType RoomType) const;
	FRuleExtension GetRandomRoomExtensionOfType(const ERoomType RoomType) const;

public:
	/* Rule collection applied when starting to generate a dungeon level
	  Set floors are generated in order, starting from index 0
	 
	  Ex. if you set this array to have 3 elements, it will generate 3 floors:
	 	First floor = PerFloorRoomStructures[0]
	 	Second floor = PerFloorRoomStructures[1]
	 	Third floor = PerFloorRoomStructures[2]
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	TArray<FRuleCollection> PerFloorRoomStructures;

	/* Rule collections used to extant base rule collection */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	TArray<FRuleExtension> RuleExtensions;
};
