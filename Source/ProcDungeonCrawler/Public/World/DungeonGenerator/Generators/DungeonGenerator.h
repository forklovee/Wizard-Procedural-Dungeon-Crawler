// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/PickupItem.h"
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"
#include "DungeonGenerator.generated.h"

class UBagComponent;
class USpellbookComponent;
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
struct FRoomData
{
	GENERATED_BODY()

	FRoomData()
	{
		Id = 0;
		RoomRules = FRuleProperties();
		RoomActor = nullptr;
		Obstacle_FromParent_Class = nullptr;
	}

	FRoomData(int NewFloorId, int NewId, FRuleProperties& NewRoomRules)
	{
		FloorId = NewFloorId;
		Id = NewId;
		
		RoomRules = NewRoomRules;
		
		RoomActor = nullptr;
		Obstacle_FromParent_Class = nullptr;
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

	bool bIsExit = false;
	bool bIsOnMainWalkthroughPath = false;
	int FloorId = 0;
	int Id = 0;
	
	FRuleProperties RoomRules;

	TSubclassOf<AObstacle> Obstacle_FromParent_Class = nullptr;
	int ObstacleSolutionRoomId = -1;

	TArray<TSubclassOf<AActor>> RequiredAssetsToSpawn;
	
	FRoomData* Parent;
	TArray<FRoomData*> Children;
	
	TMap<FVector, TArray<FWallRange>> OccupiedWallsRanges;
	
	TWeakObjectPtr<ADungeonRoom> RoomActor;
};

USTRUCT()
struct FFloorData
{
	GENERATED_BODY()

	FFloorData()
	{
		
	}
	FFloorData(const int FloorId)
	{
		Id = FloorId;
	}
	
	int Id = 0;
	int MaxRoomAmount = 0;
	TArray<FRoomData*> Rooms;

	TArray<FRoomData*> BranchRoots;
	
	TArray<FRoomData*> ObstacleRooms;
	TArray<FRoomData*> ObstacleSolverRooms;
};

USTRUCT()
struct FDungeonObstacle
{
	GENERATED_BODY()

	FDungeonObstacle()
	{
		
	}
	
	FDungeonObstacle(TSubclassOf<AActor> NewObstacleClass,
		TSubclassOf<APickupItem> NewRequiredPickup, FName NewSolverRequiredTag,
		TSubclassOf<ASpellCast> NewRequiredSpellCast)
	{
		ObstacleClass = NewObstacleClass;
		RequiredPickup = NewRequiredPickup;
		SolverRequiredTag = NewSolverRequiredTag;
		RequiredSpellCast = NewRequiredSpellCast;
	}

	TSubclassOf<AActor> ObstacleClass;
	TSubclassOf<APickupItem> RequiredPickup;
	FName SolverRequiredTag;
	
	TSubclassOf<ASpellCast> RequiredSpellCast;
};

UCLASS()
class PROCDUNGEONCRAWLER_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADungeonGenerator();

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	bool GenerateDungeon(AWizardPlayer* Player);

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	bool BuildDungeon();
	
protected:
	virtual void BeginPlay() override;

	bool LoadAndSetDungeonData();
	bool ExtendFloorRoomTree(FFloorData& FloorData, const int FloorStartRoomId, const int FloorEndRoomId);
	TArray<FRoomData*> ConnectRuleCollectionToRooms(int ParentRoomId, const int FloorId, FRuleCollection& RuleCollection, const bool bIsMainWalkthroughPath = false);
	FRoomData* SetSolverAndObstacleRoom(FRoomData& ObstacleRoom, TArray<FRoomData*>& FloorRoomBranch);

	FObstacleData* GetRandomObstacleData();
	FObstacleData* GetObstacleDataByObstacleAsset(const TSubclassOf<AObstacle> ObstacleClass);

	FObstacleData* GetObstacleData(TArray<FObstacleData*> PossibleObstacleData) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWalkthroughPath> WalkthroughPathClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDungeonConfig> DungeonConfigDataAsset;
	
protected:
	TArray<FRoomData> Rooms;
	TArray<FFloorData> Floors;
	TArray<FRoomData*> MainWalkthroughPath;
	
	TArray<TSubclassOf<ARoomPCGGlobalVolume>> PCGRoomVolumes;
	TArray<TArray<FRoomData>> DungeonLevels;

	UPROPERTY()
	UDungeonConfig* DungeonConfig;
	UPROPERTY()
	UDungeonRuleDictionary* DungeonRuleDictionary;
	UPROPERTY()
	UDungeonRoomDictionary* DungeonRoomDictionary;

	TMap<TSubclassOf<AObstacle>, FObstacleData*> ObstacleSolverMap;

	TWeakObjectPtr<UBagComponent> PlayerBag;
	TWeakObjectPtr<USpellbookComponent> PlayerSpellBook;
	
private:
	int CurrentGeneratedLevel = 0;
};
