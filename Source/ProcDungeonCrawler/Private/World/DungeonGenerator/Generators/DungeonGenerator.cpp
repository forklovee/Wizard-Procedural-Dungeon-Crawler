#include "World/DungeonGenerator/Generators/DungeonGenerator.h"

#include "PCGComponent.h"
#include "Characters/Player/WizardPlayer.h"
#include "Characters/Wizard/WizardCharacter.h"
#include "World/DungeonGenerator/DataAssets/DungeonConfig.h"
#include "World/DungeonGenerator/DataAssets/DungeonRoomDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"

#include "PCG/Public/PCGGraph.h"
#include "PCG/Public/PCGVolume.h"
#include "Tools/SplineTools.h"
#include "World/DungeonGenerator/Path/WalkthroughPath.h"
#include "World/DungeonGenerator/Rooms/RoomPCGGlobalVolume.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
}

bool ADungeonGenerator::GenerateDungeon()
{
	if (!LoadAndSetDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}

	// Create start room

	const TMap<int, TArray<FRuleCollection>>& PerFloorRoomStructures = DungeonRuleDictionary->PerFloorRoomStructures;
	const int DungeonFloorAmount = PerFloorRoomStructures.Num();
	if (DungeonFloorAmount == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No floors in Dungeon!"));
		return false;
	}
	
	for (int FloorId = 0; FloorId < DungeonFloorAmount; FloorId++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Generating Floor %i"), FloorId);
		if (!DungeonRuleDictionary->HasFloorRoomStructures(FloorId))
		{
			UE_LOG(LogTemp, Error, TEXT("No Room Structures for floor %i! Cancelling generation!"), FloorId);
			return false;
		}
		
		// Add Floor Base Structure
		FRuleCollection& FloorStructure = DungeonRuleDictionary->GetRandomFloorRoomStructure(FloorId);

		// If this is a first floor, do not connect first room to parent, because it's not existent!
		int FloorStartRoomId = FloorId == 0 ? -1 : Rooms.Num();
		ConnectRuleCollectionToRooms(FloorStartRoomId, FloorId, FloorStructure);
		const int FloorLastRoomId = FloorStartRoomId + FloorStructure.Rules.Num();

		// If it's a first room in the dungeon, reset the start room id to 0 from -1
		if (FloorStartRoomId < 0)
		{
			FloorStartRoomId = 0;
		}
		
		if (ExtendFloorRoomTree(FloorId, FloorStartRoomId, FloorLastRoomId))
		{
			UE_LOG(LogTemp, Warning, TEXT("\t - Room Tree Extension Successfull! Floor %i has %i rooms."), FloorId, Rooms.Num() - FloorStartRoomId);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Generated rooms: %i"), Rooms.Num());
	
	return Rooms.Num() > 1;
}

bool ADungeonGenerator::BuildDungeon()
{
	if (!LoadAndSetDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}
	if (WalkthroughPathClass == nullptr)
	{
		return false;
	}
	//
	// AWalkthroughPath* MainWalkthroughPath = GetWorld()->SpawnActor<AWalkthroughPath>(WalkthroughPathClass);
	// for (int RoomIdx = 0; RoomIdx < Rooms.Num(); RoomIdx++)
	// {
	// 	// get generated room data
	// 	FGenRoomData& RoomData = Rooms[RoomIdx];
	//
	// 	//debug
	// 	UE_LOG(LogTemp, Warning, TEXT("Room %i: %s"), RoomData.Id, *UEnum::GetValueAsString(RoomData.RoomType));
	//
	// 	// get room class to spawn a premade room actor
	// 	FRoomResourceEntry RoomResource = DungeonRoomDictionary->GetRandomRoomClassOfType(RoomData.RoomType);
	// 	if (RoomResource.RoomClass == nullptr)
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("Room class is null!"));
	// 		return false;
	// 	}
	//
	// 	ADungeonRoom* DungeonRoom = Cast<ADungeonRoom>(GetWorld()->SpawnActor(RoomResource.RoomClass));
	// 	if (DungeonRoom == nullptr)
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Dungeon room!"));
	// 		return false;
	// 	}
	// 	
	// 	if (RoomResource.PCGRoomVolumeClass != nullptr)
	// 	{
	// 		if (!PCGRoomVolumes.Contains(RoomResource.PCGRoomVolumeClass))
	// 		{
	// 			PCGRoomVolumes.Add(RoomResource.PCGRoomVolumeClass);
	// 		}
	// 		FString VolumeStringName = RoomResource.PCGRoomVolumeClass->GetName();
	// 		DungeonRoom->Tags.Add(FName(VolumeStringName));
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("VOLUME CLASS NULL!!!"));
	// 	}
	// 	
	// 	RoomData.RoomActor = DungeonRoom;
	//
	// 	// Start room position isn't calculated
	// 	if (RoomData.Id == 0){
	// 		DungeonRoom->SetActorLocation(-DungeonRoom->GetRoomCenter());
	// 		continue;
	// 	}
	//
	// 	// get last room data
	// 	FGenRoomData& LastRoomData = Rooms[RoomIdx-1];
	//
	// 	// get current room walls data
	// 	TArray<FRoomWall> ThisRoomWalls = RoomData.RoomActor->GetRoomWalls();
	//
	// 	bool bCorrectLocationFound = false;
	// 	while (ThisRoomWalls.Num() > 0)
	// 	{
	// 		if (bCorrectLocationFound)
	// 		{
	// 			break;
	// 		}
	// 		const int RandomWallIndex = FMath::RandRange(0, ThisRoomWalls.Num()-1);
	// 		UE_LOG(LogTemp, Display, TEXT("\t Room %i Selected Random Wall of Idx %i out of %i"), RoomIdx, RandomWallIndex, ThisRoomWalls.Num());
	//
	// 		const FRoomWall& ThisRoomWall = ThisRoomWalls[RandomWallIndex];
	// 		const FVector ThisWallNormal = ThisRoomWall.GetWallNormal();
	// 		// invert current room random wall normal to get last room desired wall normal
	// 		FVector LastRoomDesiredWallNormal = -ThisWallNormal;
	//
	// 		TArray<FRoomWall> LastRoomWalls = LastRoomData.RoomActor->GetRoomWallsOfNormal(LastRoomDesiredWallNormal);
	// 		if (LastRoomWalls.Num() == 0)
	// 		{
	// 			ThisRoomWalls.RemoveAt(RandomWallIndex);
	// 			UE_LOG(LogTemp, Error, TEXT("No compatible walls found for new room!"));
	// 			continue;
	// 		}
	//
	// 		
	// 		for (FRoomWall& LastRoomWall: LastRoomWalls)
	// 		{
	// 			FVector ThisWallStart = ThisRoomWall.StartPoint;
	// 			FVector ThisWallEnd = ThisRoomWall.EndPoint;
	// 			
	// 			// Check point directions from center to determine which point starts the wall - to the left
	// 			TArray<FVector> LastWallPointDirections = LastRoomWall.GetPointDirectionsFromWallCenter();
	// 			TArray<FVector> ThisWallPointDirections = ThisRoomWall.GetPointDirectionsFromWallCenter();
	// 			if (!LastWallPointDirections[0].Equals(ThisWallPointDirections[0], 0.1))
	// 			{
	// 				// invert wall start point
	// 				ThisWallStart = ThisRoomWall.EndPoint;
	// 				ThisWallEnd = ThisRoomWall.StartPoint;
	// 			}
	// 			
	// 			FVector ThisRoomLocation = LastRoomData.RoomActor->GetActorLocation() + LastRoomWall.StartPoint - ThisWallStart;
	// 			ThisRoomLocation -= LastRoomDesiredWallNormal * 200.f;
	// 			DungeonRoom->SetActorLocation(ThisRoomLocation);
	// 			// Check this wall intersections with last room walls
	// 			bool bIsOverlapping = false;
	// 			for (FRoomWall& TestLastRoomWall: LastRoomData.RoomActor->GetRoomWalls())
	// 			{
	// 				if (DungeonRoom->IsOverlappingWithRoom(ThisRoomWall, LastRoomData.RoomActor.Get(), LastRoomWall))
	// 				{
	// 					UE_LOG(LogTemp, Error, TEXT("Room %i is overlapping!"), RoomData.Id);
	// 					bIsOverlapping = true;
	// 					break;
	// 				}
	// 			}
	//
	// 			if (bIsOverlapping)
	// 			{
	// 				continue;
	// 			}
	// 			
	// 			MainWalkthroughPath->SplineComponent->AddSplinePoint(LastRoomData.RoomActor->GetActorLocation() + LastRoomWall.GetWallCenter(), ESplineCoordinateSpace::Local, true);
	// 			USplineTools::SetTangentsToZero(MainWalkthroughPath->SplineComponent, MainWalkthroughPath->SplineComponent->GetNumberOfSplinePoints()-1);
	// 			MainWalkthroughPath->SplineComponent->AddSplinePoint(DungeonRoom->GetActorLocation() + ThisRoomWall.GetWallCenter(), ESplineCoordinateSpace::Local, true);
	// 			USplineTools::SetTangentsToZero(MainWalkthroughPath->SplineComponent, MainWalkthroughPath->SplineComponent->GetNumberOfSplinePoints()-1);
	// 			MainWalkthroughPath->SplineComponent->AddSplinePoint(DungeonRoom->GetActorLocation() + DungeonRoom->GetRoomCenter(), ESplineCoordinateSpace::Local, true);
	// 			USplineTools::SetTangentsToZero(MainWalkthroughPath->SplineComponent, MainWalkthroughPath->SplineComponent->GetNumberOfSplinePoints()-1);
	//
	// 			// good!
	// 			bCorrectLocationFound = true;
	// 			break;
	// 		}
	// 	}
	// }
	//
	// for (TSubclassOf<ARoomPCGGlobalVolume> PcgVolume: PCGRoomVolumes)
	// {
	// 	if (PcgVolume == nullptr)
	// 	{
	// 		UE_LOG(LogTemp, Display, TEXT("Volume null!"))
	// 		continue;
	// 	}
	// 	
	// 	if (ARoomPCGGlobalVolume* RoomPcgRoom = GetWorld()->SpawnActor<ARoomPCGGlobalVolume>(PcgVolume))
	// 	{
	// 		RoomPcgRoom->SetActorScale3D(FVector3d(1000.f, 1000.f, 1000.f));
	// 	}
	// }
	//
	return true;
}

bool ADungeonGenerator::LoadAndSetDungeonData()
{
	if (DungeonConfigDataAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("DungeonConfig is null!"));
		return false;
	}

	// Load Dungeon Config
	if (DungeonConfig == nullptr)
	{
		DungeonConfig = DungeonConfigDataAsset.LoadSynchronous();
	}
	if (DungeonConfig->DungeonRuleDictionaryDataAsset.IsNull() ||
		DungeonConfig->DungeonRoomDictionaryDataAsset.IsNull() ||
		DungeonConfig->ObstacleDataTable.IsNull()
		)
	{
		UE_LOG(LogTemp, Error, TEXT("Some CRUCIAL Data Asset in Dungeon Config is NULL!"));
		return false;
	}

	// Load Dungeon Room Dictionary
	if (DungeonRoomDictionary == nullptr)
	{
		DungeonRoomDictionary = DungeonConfig->DungeonRoomDictionaryDataAsset.LoadSynchronous();
	}
	if (!DungeonRoomDictionary->HasRoomClassOfType(ERoomType::Start))
	{
		UE_LOG(LogTemp, Error, TEXT("No Start Room in Room Dictionary!"));
		return false;
	}
	
	// Load Dungeon Rule Dictionary
	if (DungeonRuleDictionary == nullptr)
	{
		DungeonRuleDictionary = DungeonConfig->DungeonRuleDictionaryDataAsset.LoadSynchronous();
	}
	if (!DungeonRuleDictionary->HasFloorRoomStructures(0))
	{
		UE_LOG(LogTemp, Error, TEXT("Rule Dictionary Has No Floor Structures!"));
		return false;
	}

	//Load Obstacle Data Table
	if (ObstacleSolverDataMap.Num() == 0)
	{
		const UDataTable* ObstacleDataTable = DungeonConfig->ObstacleDataTable.LoadSynchronous();
		TArray<FObstacleData*> SpellBookRows;
		ObstacleDataTable->GetAllRows<FObstacleData>(TEXT("Obstacle"), SpellBookRows);
		for (const FObstacleData* Row : SpellBookRows)
		{
			ObstacleSolverDataMap.Add(*Row);
		}
	}

	return true;
}

bool ADungeonGenerator::ExtendFloorRoomTree(const int FloorId, const int FloorStartRoomId, const int FloorEndRoomId)
{
	UE_LOG(LogTemp, Warning, TEXT("\t- Extanding Floor Tree"));
	if (!LoadAndSetDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}

	int MaxFloorRoomAmount = -1;
	if (!DungeonConfig->MaxRoomAmountOnFloor.Contains(FloorId))
	{
		UE_LOG(LogTemp, Error, TEXT("Dungeon Config doesn't have an entry for max rooms on floor %i, only required extensions will be applied!"));
	}
	else
	{
		MaxFloorRoomAmount = DungeonConfig->MaxRoomAmountOnFloor[FloorId];
	}

	TArray<int> RoomIdsForExtension;
	for (int RoomId = FloorStartRoomId; RoomId < FloorEndRoomId; RoomId++)
	{
		const FGenRoomData& RoomData = Rooms[RoomId];
		const FRuleProperties& RoomRules = RoomData.RoomRules;

		// Can't extend from current room
		if (!RoomRules.bMarkForExtension) continue;

		RoomIdsForExtension.Add(RoomId);
		
		// Not an obstacle or start room, add on stack
		if (!RoomRules.bHasObstacle)
		{
			continue;
		}

		// Is an obstacle, get random room from stack
		if (RoomIdsForExtension.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No rooms to extend from! Check Set Rules Data Asset!"));
			return false;
		}
		const int RandomRoomId = FMath::RandRange(0, RoomIdsForExtension.Num()-1);
		const FGenRoomData& RandomRoomData = Rooms[RoomIdsForExtension[RandomRoomId]];
		
		const ERoomType RoomType = RandomRoomData.RoomRules.RoomType;
		if (RoomType == ERoomType::None)
		{
			UE_LOG(LogTemp, Error, TEXT("Room type of room %i is None!"), RandomRoomId);
			return false;
		}

		// Get random collection for the chosen room
		if (!DungeonRuleDictionary->HasRoomExtensionOfType(RoomType))
		{
			UE_LOG(LogTemp, Error, TEXT("No Room Collection for Type of %s"), *UEnum::GetValueAsString(RoomType));
			return false;
		}
		
		FRuleCollection RuleCollection = DungeonRuleDictionary->GetRandomRoomExtensionOfType(RoomType).Rules;
		TArray<FGenRoomData*> NewRooms = ConnectRuleCollectionToRooms(RandomRoomId, FloorId, RuleCollection);

		// Choose Obstacle Solution Room Randomly
		FGenRoomData* ObstacleSolver = SetSolverAndObstacleRoom(RoomId, NewRooms);
		
	}

	return true;
}

TArray<FGenRoomData*> ADungeonGenerator::ConnectRuleCollectionToRooms(const int ParentRoomId, const int FloorId, FRuleCollection& RuleCollection)
{
	TArray<FGenRoomData*> NewRooms;
	for (int RuleIdx = 0; RuleIdx < RuleCollection.Rules.Num(); RuleIdx++)
	{
		FRuleProperties& RuleProperties = RuleCollection.Rules[RuleIdx];

		// Create room data
		const int NewRoomId = Rooms.Add(
			FGenRoomData(FloorId, Rooms.Num(), RuleProperties)
			);
		FGenRoomData& RoomData = Rooms[NewRoomId];
		NewRooms.Add(&RoomData);
		// Connect first room to create a branch
		if (ParentRoomId > -1 && RuleIdx == 0)
		{
			RoomData.Parent = &Rooms[ParentRoomId];
			continue;
		}

		// Create a Parent-Child relation
		if (RuleIdx > 0)
		{
			RoomData.Parent = &Rooms[RuleIdx - 1];
			RoomData.Parent->Children.Add(&RoomData);
		}
	}
	return NewRooms;
}

FGenRoomData* ADungeonGenerator::SetSolverAndObstacleRoom(int ObstacleRoomId,
	TArray<FGenRoomData*>& FloorRoomBranch) const
{
	TArray<FGenRoomData*> PossibleObstacleSolvers;
	for (FGenRoomData* NewRoom: FloorRoomBranch)
	{
		if (NewRoom->RoomRules.bCanHaveObstacleSolver)
		{
			PossibleObstacleSolvers.Add(NewRoom);
		}
	}

	FGenRoomData* ObstacleSolver;
	if (PossibleObstacleSolvers.Num() > 0)
	{
		// solver cannot be placed in the first room of the branch!
		const int RandomObstacleSolverIdx = FMath::RandRange(1, PossibleObstacleSolvers.Num()-1);
		ObstacleSolver = PossibleObstacleSolvers[RandomObstacleSolverIdx];
		ObstacleSolver->ObstacleSolutionRoomId = ObstacleRoomId;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Room Marked As Obstacle Solver! Choosing From Branch..."))
	
		// If no room is marked to have an obstacle solver, choose a random room from the branch	
		const int RandomObstacleSolverIdx = FMath::RandRange(1, FloorRoomBranch.Num()-1);
		FGenRoomData* ObstacleSolver = PossibleObstacleSolvers[RandomObstacleSolverIdx];
		ObstacleSolver->ObstacleSolutionRoomId = ObstacleRoomId;
	}
	
	//Todo: Choose Solver
	
	
	return ObstacleSolver;
}

