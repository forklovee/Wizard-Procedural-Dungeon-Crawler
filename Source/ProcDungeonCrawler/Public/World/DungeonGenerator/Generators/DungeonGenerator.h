// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"
#include "DungeonGenerator.generated.h"

class AWizardPlayer;
class ADungeonObstacle;
class AWalkthroughPath;
class ARoomPCGGlobalVolume;
class UPCGGraph;
struct FWallRange;
class UDungeonRoomDictionary;
class UDungeonConfig;
enum class ERoomType : uint8;
class ADungeonRoom;

USTRUCT()
struct FGenRoomData
{
	GENERATED_BODY()

	FGenRoomData()
	{
		Id = 0;
		RoomRules = FRuleProperties();
		RoomActor = nullptr;
	}

	FGenRoomData(int NewFloorId, int NewId, FRuleProperties& NewRoomRules)
	{
		FloorId = NewFloorId;
		Id = NewId;
		
		RoomRules = NewRoomRules;
		
		RoomActor = nullptr;
	}
	
	bool IsWallRangeOccupied(const FVector& WallNormal, const float& StartPoint, const float& EndPoint) const
	{
		if (!OccupiedWallsRanges.Contains(WallNormal))
		{
			return false;
		}
		const TArray<FWallRange> WallOccupiedRanges = OccupiedWallsRanges[WallNormal];
		return WallOccupiedRanges.ContainsByPredicate([=](const FWallRange& CheckWallOccupiedRange)
		{
			return (CheckWallOccupiedRange.StartPoint == StartPoint && CheckWallOccupiedRange.StartPoint == EndPoint) ||
				   (CheckWallOccupiedRange.StartPoint == EndPoint && CheckWallOccupiedRange.StartPoint == StartPoint);
		});
	}
	
	bool OccupyWallRange(const FVector& WallNormal, const float& StartPoint, const float& EndPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupyWallRange: %f to %f"), StartPoint, EndPoint);
	
		if (!OccupiedWallsRanges.Contains(WallNormal))
		{
			OccupiedWallsRanges.Add(WallNormal, TArray<FWallRange>());
		}

		TArray<FWallRange>& WallOccupiedRanges = OccupiedWallsRanges[WallNormal];
		if (IsWallRangeOccupied(WallNormal, StartPoint, EndPoint))
		{
			UE_LOG(LogTemp, Error, TEXT("Room Range of %f to %f already marked as occupied!"), StartPoint, EndPoint);
			return false;
		}

		WallOccupiedRanges.Add( FWallRange(StartPoint, EndPoint) );
	
		return true;
	}

	int FloorId = 0;
	int Id = 0;
	
	FRuleProperties RoomRules;

	TSubclassOf<ADungeonObstacle> Obstacle_FromParent_Class;
	int ObstacleSolutionRoomId = -1;
	
	FGenRoomData* Parent;
	TArray<FGenRoomData*> Children;
	
	TMap<FVector, TArray<FWallRange>> OccupiedWallsRanges;
	
	TWeakObjectPtr<ADungeonRoom> RoomActor;
};

UCLASS()
class PROCDUNGEONCRAWLER_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADungeonGenerator();

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	bool GenerateDungeon();

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	bool BuildDungeon();
	
protected:
	virtual void BeginPlay() override;

	bool LoadAndSetDungeonData();
	bool ExtendFloorRoomTree(const int FloorId, const int FloorStartRoomId, const int FloorEndRoomId);
	TArray<FGenRoomData*> ConnectRuleCollectionToRooms(const int ParentRoomId, const int FloorId, FRuleCollection& RuleCollection);
	FGenRoomData* SetSolverAndObstacleRoom(int ObstacleRoomId, TArray<FGenRoomData*>& FloorRoomBranch) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWalkthroughPath> WalkthroughPathClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDungeonConfig> DungeonConfigDataAsset;
	
protected:
	TArray<FGenRoomData> Rooms;
	
	TArray<TSubclassOf<ARoomPCGGlobalVolume>> PCGRoomVolumes;
	TArray<TArray<FGenRoomData>> DungeonLevels;

	UPROPERTY()
	UDungeonConfig* DungeonConfig;
	UPROPERTY()
	UDungeonRuleDictionary* DungeonRuleDictionary;
	UPROPERTY()
	UDungeonRoomDictionary* DungeonRoomDictionary;

	TArray<FObstacleData> ObstacleSolverDataMap;

private:
	int CurrentGeneratedLevel = 0;
};
