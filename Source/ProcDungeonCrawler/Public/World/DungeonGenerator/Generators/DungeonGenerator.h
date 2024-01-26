// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "..\..\..\Items\Item.h"
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"
#include "DungeonGenerator.generated.h"

class UInventoryComponent;
class UBagComponent;
class USpellBookComponent;
class ADungeonObstacle;
class AWalkthroughPath;
class ARoomPCGGlobalVolume;
// class UPCGGraph;
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

	bool bIsExit = false;
	bool bIsOnMainWalkthroughPath = false;
	int FloorId = 0;
	int Id = 0;
	
	FRuleProperties RoomRules;

	TSubclassOf<AObstacle> Obstacle_FromParent_Class = nullptr;
	int HasSolutionToObstacleInRoomIdx = -1;

	TArray<TSubclassOf<AActor>> RequiredAssetsToSpawn;
	
	int ParentId = -1;
	TArray<int> ChildrenIds;
	
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
		TSubclassOf<AItem> NewRequiredPickup, FName NewSolverRequiredTag,
		TSubclassOf<ASpell> NewRequiredSpellCast)
	{
		ObstacleClass = NewObstacleClass;
		RequiredPickup = NewRequiredPickup;
		SolverRequiredTag = NewSolverRequiredTag;
		RequiredSpellCast = NewRequiredSpellCast;
	}

	TSubclassOf<AActor> ObstacleClass;
	TSubclassOf<AItem> RequiredPickup;
	FName SolverRequiredTag;
	
	TSubclassOf<ASpell> RequiredSpellCast;
};

UCLASS()
class PROCDUNGEONCRAWLER_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	ADungeonGenerator();

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	bool GenerateDungeon(APlayerPawn* Player);

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	bool BuildDungeon();

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void BuildRoomById(int Id);
	
protected:
	virtual void BeginPlay() override;
	
	ADungeonRoom* BuildRoom(FRoomData& RoomData, FVector& BranchDirection);
	
	bool LoadAndSetDungeonData();
	bool ExtendFloorRoomTree(FFloorData& FloorData, const int FloorStartRoomId, const int FloorEndRoomId);
	TArray<FRoomData*> ConnectRuleCollectionToRooms(int ParentRoomId, const int FloorId, FRuleCollection& RuleCollection, const bool bIsMainWalkthroughPath = false);
	FRoomData* SetSolverAndObstacleRoom(FRoomData& ObstacleRoom, TArray<FRoomData*>& FloorRoomBranch);

	FObstacleData* GetRandomObstacleData();
	FObstacleData* GetObstacleDataByObstacleAsset(const TSubclassOf<AObstacle> ObstacleClass);

	FObstacleData* GetObstacleData(TArray<FObstacleData*> PossibleObstacleData) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GridTileSize = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWalkthroughPath> WalkthroughPathClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDungeonConfig> DungeonConfigDataAsset;
	
protected:
	TArray<FRoomData> Rooms;

	TArray<ADungeonRoom*> RoomActors;
	
	TArray<FFloorData> Floors;
	TArray<FRoomData*> MainWalkthroughPath;

	UPROPERTY()
	UDungeonConfig* DungeonConfig;
	UPROPERTY()
	UDungeonRuleDictionary* DungeonRuleDictionary;
	UPROPERTY()
	UDungeonRoomDictionary* DungeonRoomDictionary;

	TMap<TSubclassOf<AObstacle>, FObstacleData*> ObstacleSolverMap;

	TWeakObjectPtr<UInventoryComponent> PlayerBag;
	TWeakObjectPtr<USpellBookComponent> PlayerSpellBook;

private:
	int CurrentGeneratedLevel = 0;

	int LastBuiltRoomId = -1;

	FTimerHandle BuildRoomTimerHandle;
};
