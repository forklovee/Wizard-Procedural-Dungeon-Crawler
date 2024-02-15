// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "..\..\..\Items\Item.h"
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"
#include "DungeonGenerator.generated.h"

class ADoor;
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
struct FDoorData
{
	GENERATED_BODY()

	TWeakObjectPtr<ADoor> DoorActor;
	FVector DoorLocation;
	float DoorRotationYaw = 0.f;
	
	TWeakObjectPtr<ADungeonRoom> RoomA;
	TWeakObjectPtr<ADungeonRoom> RoomB;
};

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

	int FloorId = 0;
	int Id = 0;
	int ParentId = -1;
	TArray<FRoomData*> Children;
	
	FRuleProperties RoomRules;
	
	bool bIsExit = false;
	bool bIsOnMainWalkthroughPath = false;

	FVector BranchDirection = FVector::ZeroVector;
	
	TSubclassOf<AObstacle> Obstacle_FromParent_Class = nullptr;
	int HasSolutionToObstacleInRoomIdx = -1;

	TArray<TSubclassOf<AActor>> RequiredAssetsToSpawn;
	
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
	FVector MainBranchDirection = FVector::ZeroVector;
	
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

	UFUNCTION(BlueprintCallable, Category="Dungeon|Debug")
	void PrintDungeonStructure();

	UFUNCTION(BlueprintCallable, Category="Dungeon|Debug")
	void DrawDungeonDebugShapes();
	
	UFUNCTION(BlueprintCallable, Category="Dungeon|Generation")
	bool GenerateDungeon();

	UFUNCTION(BlueprintCallable, Category="Dungeon|Generation")
	bool BuildDungeon(float NewGridTileSize, float NewMeshTileSize);

	UFUNCTION(BlueprintCallable, Category="Dungeon|Gameplay")
	FTransform GetPlayerStartingTransform() const;
	
	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void BuildRoomById(int Id);
	
protected:
	virtual void BeginPlay() override;
	
	ADungeonRoom* BuildRoom(FRoomData& RoomData);
	bool SetValidRoomLocation(FRoomData& RoomData, TArray<FVector>& PositionChecks);
	TArray<FVector> GetPositionCheckPoints(const FRoomWall* ThisRoomWall, const FRoomData& ParentRoomData, const FRoomWall* ParentRoomWall) const;
	
	bool LoadAndSetDungeonData();
	bool ExtendFloorRoomTree(FFloorData& FloorData, const int FloorStartRoomId, const int FloorEndRoomId);
	TArray<FRoomData*> ConnectRuleCollectionToRooms(int ParentRoomId, FRuleCollection& RuleCollection, const bool bIsMainWalkthroughPath = false);
	FRoomData* SetSolverAndObstacleRoom(FRoomData& ObstacleRoom, TArray<FRoomData*>& FloorRoomBranch);

	FObstacleData* GetRandomObstacleData();
	FObstacleData* GetObstacleDataByObstacleAsset(const TSubclassOf<AObstacle> ObstacleClass);

	FObstacleData* GetObstacleData(TArray<FObstacleData*> PossibleObstacleData) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GridSize = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeshSize = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWalkthroughPath> WalkthroughPathClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDungeonConfig> DungeonConfigDataAsset;
	
protected:
	TArray<FRoomData> Rooms;
	TArray<FDoorData> Doors;

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
	FVector DefaultBranchDirection = FVector::ForwardVector;
	
	int CurrentGeneratedLevel = 0;
	int LastBuiltRoomId = -1;

	FTimerHandle BuildRoomTimerHandle;
};


