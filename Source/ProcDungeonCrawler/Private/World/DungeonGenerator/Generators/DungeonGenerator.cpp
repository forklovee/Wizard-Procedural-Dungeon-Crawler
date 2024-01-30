#include "World/DungeonGenerator/Generators/DungeonGenerator.h"

#include "FrameTypes.h"
#include "Characters/Human/Player/PlayerPawn.h"
#include "Components/Character/InventoryComponent.h"
#include "Components/Character/SpellbookComponent.h"
#include "Spell/Rune.h"
#include "Spell/Spell.h"
#include "World/DungeonGenerator/DataAssets/DungeonConfig.h"
#include "World/DungeonGenerator/DataAssets/DungeonRoomDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"

#include "World/DungeonGenerator/Actors/Obstacles/Obstacle.h"
#include "World/DungeonGenerator/Path/WalkthroughPath.h"

ADungeonGenerator::ADungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADungeonGenerator::PrintDungeonStructure()
{
	for (const FRoomData& RoomData: Rooms)
	{
		UE_LOG(LogTemp, Display, TEXT("\t %i - %s"), RoomData.Id, *UEnum::GetValueAsString(RoomData.RoomRules.RoomType));
			
		// Print Parent
		if (RoomData.ParentId != -1)
		{
			UE_LOG(LogTemp, Display, TEXT("\t\t Parent: %i - %s"), RoomData.ParentId, *UEnum::GetValueAsString(Rooms[RoomData.ParentId].RoomRules.RoomType));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("\t\t Dungeon Start"));
		}

		// // Print Obstacle Data
		// if (RoomData->Obstacle_FromParent_Class != nullptr)
		// {
		// 	UE_LOG(LogTemp, Display, TEXT("\t\t  Has Obstacle of: %s"), *RoomData->Obstacle_FromParent_Class->GetName());
		// }
		// if (RoomData->HasSolutionToObstacleInRoomIdx > -1)
		// {
		// 	UE_LOG(LogTemp, Display, TEXT("\t\t  Has Obstacle Solution for Room of Id: %i"), RoomData->HasSolutionToObstacleInRoomIdx);
		// }
		//
		// // Print Set Asset Data
		// if (RoomData->RequiredAssetsToSpawn.Num() > 0)
		// {
		// 	UE_LOG(LogTemp, Display, TEXT("\t\t  Has Assets to Spawn:"));
		// 	for (const TSubclassOf<AActor> AssetClass: RoomData->RequiredAssetsToSpawn)
		// 	{
		// 		UE_LOG(LogTemp, Display, TEXT("\t\t\t- %s"), *AssetClass->GetName());
		// 	}
		// }

		// Print Children
		if (RoomData.Children.Num() > 0)
		{
			UE_LOG(LogTemp, Display, TEXT("\t\t  Children: (%i)"), RoomData.Children.Num());
			for (FRoomData* Child: RoomData.Children)
			{
				UE_LOG(LogTemp, Display, TEXT("\t\t\t %i - %s"), Child->Id, *UEnum::GetValueAsString(Child->RoomRules.RoomType));
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("\t\t -Branch End-"));
		}
	}
}

void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
}

bool ADungeonGenerator::GenerateDungeon(APlayerPawn* Player)
{
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: Failed to fetch player pawn"));
		return false;
	}
	PlayerBag = Player->Inventory;
	PlayerSpellBook = Player->SpellBook;
	
	if (!LoadAndSetDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}
	
	const int DungeonFloorAmount = DungeonRuleDictionary->GetFloorAmount();
	if (DungeonFloorAmount == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No floors in Dungeon!"));
		return false;
	}
	int FloorId = 0;
	
	UE_LOG(LogTemp, Warning, TEXT("Generating Floor %i"), FloorId);
	
	FFloorData& FloorData = Floors[
		Floors.Add(
		FFloorData(FloorId)
	)
	];
	
	if (!DungeonConfig->MaxRoomAmountOnFloor.Contains(FloorData.Id))
	{
		UE_LOG(LogTemp, Error, TEXT("Dungeon Config doesn't have an entry for max rooms on floor %i, only required extensions will be applied!"));
	}
	else
	{
		FloorData.MaxRoomAmount = DungeonConfig->MaxRoomAmountOnFloor[FloorId];
	}
	
	if (!DungeonRuleDictionary->HasFloorRoomStructures(FloorId))
	{
		UE_LOG(LogTemp, Error, TEXT("No Room Structures for floor %i! Cancelling generation!"), FloorId);
		return false;
	}
	
	// Add Floor Base Structure
	FRuleCollection& FloorStructure = DungeonRuleDictionary->GetFloorRoomStructure(FloorId);

	// If this is a first floor, do not connect first room to parent, because it's not existent!
	int FloorStartRoomId = FloorId == 0 ? -1 : Rooms.Num()-1;
	FloorData.Rooms = ConnectRuleCollectionToRooms(FloorStartRoomId, FloorId, FloorStructure, true);
	
	const int FloorLastRoomId = FloorStartRoomId + FloorStructure.Rules.Num();

	// If it's a first room in the dungeon, reset the start room id to 0 from -1
	if (FloorStartRoomId < 0)
	{
		FloorStartRoomId = 0;
	}
	
	if (ExtendFloorRoomTree(FloorData, FloorStartRoomId, FloorLastRoomId))
	{
		//successfully extended floor tree
		PrintDungeonStructure();
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
	// if (WalkthroughPathClass == nullptr)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("WalkthroughPathClass is null!"));
	// 	return false;
	// }
	//
	// const AWalkthroughPath* SplineGlobalWalkthough = GetWorld()->SpawnActor<AWalkthroughPath>(WalkthroughPathClass);

	FRoomData& StartRoomData = Rooms[0];
	FVector BranchDirection = FVector(1.f, -1.f, 0.f);
	for (int RoomIdx = 0; RoomIdx < Rooms.Num(); RoomIdx++)
	{
		if (ADungeonRoom* NewRoom = BuildRoom(Rooms[RoomIdx], BranchDirection))
		{
			FString ActorName = FString::Printf(TEXT("Room_%i"), RoomIdx);
			NewRoom->SetActorLabel(ActorName);
		}
	}
	
	return true;
}

void ADungeonGenerator::BuildRoomById(int Id)
{
	// BuildRoom(Rooms[Id]);
}

ADungeonRoom* ADungeonGenerator::BuildRoom(FRoomData& RoomData, FVector& BranchDirection)
{
	UE_LOG(LogTemp, Warning, TEXT("Room %i: %s"), RoomData.Id, *UEnum::GetValueAsString(RoomData.RoomRules.RoomType));

	// get room class to spawn a premade room actor
	FRoomResourceEntry RoomResource = DungeonRoomDictionary->GetRandomRoomClassOfType(RoomData.RoomRules.RoomType);
	if (RoomResource.RoomClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Room class is null!"));
		return nullptr;
	}

	//Spawn Room Actor
	RoomData.RoomActor = Cast<ADungeonRoom>(GetWorld()->SpawnActor(RoomResource.RoomClass.Get()));
	if (!RoomData.RoomActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn Dungeon room!"));
		return nullptr;
	}

	//Add to Dungeon Room Actors
	RoomActors.Add(RoomData.RoomActor.Get());
	if (RoomData.ParentId == -1){
		RoomData.RoomActor->SetActorLocation(-RoomData.RoomActor->GetRoomCenter());
		return RoomData.RoomActor.Get();
	}
	
	// get parent room data
	const FRoomData& ParentRoomData = Rooms[RoomData.ParentId];
	if (!ParentRoomData.RoomActor.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Parent Room of Id: %i, has no spawned room!"), ParentRoomData.Id);
		return nullptr;
	}
	
	// get current room walls data
	TMap<FVector, FRoomWall*> CurrentRoomWallsMap;
	TArray<FVector> SortedRoomWallNormals;
	for (FRoomWall* RoomWall: RoomData.RoomActor->GetValidRoomWalls())
	{
		const FVector WallNormal = RoomWall->GetWallNormal();
		const float DotToDesired = FVector::DotProduct(WallNormal, -BranchDirection.GetSafeNormal());
		CurrentRoomWallsMap.Add(WallNormal, RoomWall);
		
		if (SortedRoomWallNormals.Num() == 0)
		{
			SortedRoomWallNormals.Add(WallNormal);
		}
		else
		{
			const float FirstNormalDot = FVector::DotProduct(SortedRoomWallNormals[0], -BranchDirection.GetSafeNormal());
			if (DotToDesired > FirstNormalDot)
			{
				SortedRoomWallNormals.Insert(WallNormal, 0);
			}
			else
			{
				SortedRoomWallNormals.Add(WallNormal);
			}
		}
	}
	
	bool bCorrectLocationFound = false;
	
	for(FVector& ThisWallNormal: SortedRoomWallNormals)
	{
		if (bCorrectLocationFound)
		{
			break;
		}

		FRoomWall* ThisRoomWall = CurrentRoomWallsMap[ThisWallNormal];
		
		// invert current room random wall normal to get last room desired wall normal
		FVector ParentRoomDesiredWallNormal = -ThisWallNormal;
		TArray<FRoomWall*> ParentRoomWalls = ParentRoomData.RoomActor->GetRoomWallsOfNormal(ParentRoomDesiredWallNormal);
		
		for (FRoomWall* ParentRoomWall: ParentRoomWalls)
		{
			float RoomRotationAngle = FMath::RadiansToDegrees(FMath::Acos(ThisWallNormal.Dot(ParentRoomWall->GetWallNormal())));
			RoomData.RoomActor->SetActorRotation(FRotator(0.f, 0.f, RoomRotationAngle));
			
			FVector ThisWallStart = ThisRoomWall->StartPoint;
			FVector ThisWallEnd = ThisRoomWall->EndPoint;
			
			// Check point directions from center to determine which point starts the wall - to the left
			TArray<FVector> LastWallPointDirections = ParentRoomWall->GetPointDirectionsFromWallCenter();
			TArray<FVector> ThisWallPointDirections = ThisRoomWall->GetPointDirectionsFromWallCenter();
			if (!LastWallPointDirections[0].Equals(ThisWallPointDirections[0], 0.1))
			{
				// invert wall start point
				ThisWallStart = ThisRoomWall->EndPoint;
				ThisWallEnd = ThisRoomWall->StartPoint;
			}
			
			FVector ThisRoomStartLocation = ParentRoomData.RoomActor->GetActorLocation() + ParentRoomWall->StartPoint - ThisWallStart;
			FVector ThisRoomEndLocation = ParentRoomData.RoomActor->GetActorLocation() + ParentRoomWall->EndPoint - ThisWallEnd;

			bool bIsOverlapping = false;
			for (float CurrentLerpAlpha = 0.f; CurrentLerpAlpha <= 1.f; CurrentLerpAlpha += 0.25f)
			{
				FVector ThisRoomLocationCheck = FMath::Lerp(ThisRoomStartLocation, ThisRoomEndLocation, CurrentLerpAlpha);
				RoomData.RoomActor->SetActorLocation(ThisRoomLocationCheck);
				
				for (ADungeonRoom* OtherRoom: RoomActors)
				{
					if (OtherRoom == RoomData.RoomActor.Get()) continue;

					bIsOverlapping = RoomData.RoomActor->IsOverlappingWithRoom(OtherRoom);
					if (bIsOverlapping)
					{
						break;
					}
				}
			}
			
			if (bIsOverlapping)
			{
				continue;
			}
			
			// good!
			bCorrectLocationFound = true;
			break;
		}
	}

	for (ADungeonRoom* RoomActor: RoomActors)
	{
		RoomActor->DrawDebugShapes();
	}

	return RoomData.RoomActor.Get();
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

	//Todo: Load Obstacle Data Table
	if (ObstacleSolverMap.Num() == 0)
	{
		const UDataTable* ObstacleDataTable = DungeonConfig->ObstacleDataTable.LoadSynchronous();
		TArray<FObstacleData*> SpellBookRows;
		ObstacleDataTable->GetAllRows<FObstacleData>(TEXT("Obstacle"), SpellBookRows);
		for (FObstacleData* Row : SpellBookRows)
		{
			if (Row->PreferredObstacle == nullptr) continue;
			
			ObstacleSolverMap.Add(Row->PreferredObstacle, Row);
		}
	}

	return true;
}

bool ADungeonGenerator::ExtendFloorRoomTree(FFloorData& FloorData, const int FloorStartRoomId, const int FloorEndRoomId)
{
	UE_LOG(LogTemp, Warning, TEXT("\t- Extanding Floor Tree of %i Rooms!"), FloorData.Rooms.Num());
	if (!LoadAndSetDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}
	
	int BranchCurrentRoomId = -1; // starting from -1 to increment early
	// Place out obstacle solvers
	TArray<int> RoomIdsForExtension;
	for (int RoomId = FloorStartRoomId; RoomId < FloorEndRoomId; RoomId++)
	{
		// incrementing now, because of continues that would skip it
		BranchCurrentRoomId++;
		
		FRoomData& RoomData = Rooms[RoomId];
		const FRuleProperties& RoomRules = RoomData.RoomRules;

		// Can't extend from current room
		if (!RoomRules.bMarkForExtension) continue;

		const ERoomType RoomType = RoomData.RoomRules.RoomType;

		// Get random collection for the chosen room
		if (!DungeonRuleDictionary->HasRoomExtensionOfType(RoomType))
		{
			UE_LOG(LogTemp, Error, TEXT("No Room Collection for Type of %s"), *UEnum::GetValueAsString(RoomType));
			return false;
		}
		
		FRuleCollection RuleCollection = DungeonRuleDictionary->GetRandomRoomExtensionOfType(RoomType).Rules;
		TArray<FRoomData*> NewRooms = ConnectRuleCollectionToRooms(RoomId, FloorData.Id, RuleCollection);
		// Merge New Rooms to Floor Data Rooms
		for (FRoomData* NewRoom: NewRooms)
		{
			FloorData.Rooms.Add(NewRoom);
		}
	}

	return true;
}

TArray<FRoomData*> ADungeonGenerator::ConnectRuleCollectionToRooms(int ParentRoomId, const int FloorId, FRuleCollection& RuleCollection,
	 const bool bIsMainWalkthroughPath)
{
	UE_LOG(LogTemp, Display, TEXT("\t - Creating Branch with Id: %i as Root!"), ParentRoomId)
	
	TArray<FRoomData*> NewRooms;
	for (int RuleIdx = 0; RuleIdx < RuleCollection.Rules.Num(); RuleIdx++)
	{
		FRuleProperties& RuleProperties = RuleCollection.Rules[RuleIdx];

		// Create room data
		const int NewRoomId = Rooms.Add(
			FRoomData(FloorId, Rooms.Num(), RuleProperties)
			);
		FRoomData& RoomData = Rooms[NewRoomId];
		NewRooms.Add(&RoomData);
		
		// Add to Main Walkthrough path if marked
		RoomData.bIsOnMainWalkthroughPath = bIsMainWalkthroughPath;
		if (bIsMainWalkthroughPath)
		{
			MainWalkthroughPath.Add(&RoomData);
		}

		// Start Room of entire Dungeon, omit
		if (ParentRoomId == -1 && RuleIdx == 0)
		{
			ParentRoomId = NewRoomId;
			RoomData.ParentId = -1;
			continue;
		}
		
		// Create a Parent-Child relation
		RoomData.ParentId = ParentRoomId;
		// Todo: Why it disappears on original parent room?
		Rooms[ParentRoomId].Children.Add(&RoomData);
		
		// ParentRoomId Changes with each iteration
		ParentRoomId = NewRoomId;
		
		// If first room of the branch, add to branch roots
		if (RuleIdx == 0 && !bIsMainWalkthroughPath)
		{
			Floors[FloorId].BranchRoots.Add(&RoomData);
		}
	}
	return NewRooms;
}

FRoomData* ADungeonGenerator::SetSolverAndObstacleRoom(FRoomData& ObstacleRoom,
	TArray<FRoomData*>& FloorRoomBranch)
{
	if (FloorRoomBranch.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Floor branch is too short to have an obstacle solver!"))
		return &ObstacleRoom;
	}
	
	TArray<FRoomData*> PossibleObstacleSolvers;
	for (FRoomData* NewRoom: FloorRoomBranch)
	{
		if (NewRoom->RoomRules.bCanHaveObstacleSolver)
		{
			PossibleObstacleSolvers.Add(NewRoom);
		}
	}

	// Choose Obstacle Solver Room Randomly
	FRoomData* ObstacleSolverRoom;
	if (PossibleObstacleSolvers.Num() > 0)
	{
		// solver cannot be placed in the first room of the branch!
		const int RandomObstacleSolverIdx = FMath::RandRange(1, PossibleObstacleSolvers.Num()-1);
		ObstacleSolverRoom = PossibleObstacleSolvers[RandomObstacleSolverIdx];
		ObstacleSolverRoom->HasSolutionToObstacleInRoomIdx = ObstacleRoom.Id;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Room Marked As Obstacle Solver! Choosing From Branch..."))
	
		// If no room is marked to have an obstacle solver, choose a random room from the branch	
		const int RandomObstacleSolverIdx = FMath::RandRange(1, FloorRoomBranch.Num()-1);
		ObstacleSolverRoom = FloorRoomBranch[RandomObstacleSolverIdx];
		ObstacleSolverRoom->HasSolutionToObstacleInRoomIdx = ObstacleRoom.Id;
	}

	//Choose Best Lowest Effort Solver

	const FObstacleData* ObstacleData = ObstacleRoom.RoomRules.PreferredObstacle == nullptr
	? GetRandomObstacleData()
	: GetObstacleDataByObstacleAsset(ObstacleRoom.RoomRules.PreferredObstacle);

	if (ObstacleData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find any obstacle data! Please Fill Obstacle Data Table!"))
		return &ObstacleRoom;
	}
	
	//Todo
	ObstacleRoom.Obstacle_FromParent_Class = ObstacleData->PreferredObstacle;
	ObstacleRoom.HasSolutionToObstacleInRoomIdx = ObstacleSolverRoom->Id;

	if (ObstacleData->RequiredSpellCast != nullptr)
	{
		TArray<URune*> MissingRunes;
		if (!PlayerSpellBook->CanSpellBeCasted(ObstacleData->RequiredSpellCast, MissingRunes))
		{
			for (const URune* RuneCast: MissingRunes)
			{
				ObstacleSolverRoom->RequiredAssetsToSpawn.Add(RuneCast->GetClass());
			}
		}
	}
	else if (ObstacleData->RequiredPickup != nullptr)
	{
		ObstacleSolverRoom->RequiredAssetsToSpawn.Add(ObstacleData->RequiredPickup);
	}
	
	// else
	// {
	// 	//Todo: Handle no obstacle solver found
	// }
	
	return ObstacleSolverRoom;
}

FObstacleData* ADungeonGenerator::GetRandomObstacleData()
{
	TArray<FObstacleData*> PossibleObstacleData;
	ObstacleSolverMap.GenerateValueArray(PossibleObstacleData);
	return GetObstacleData(PossibleObstacleData);
}

FObstacleData* ADungeonGenerator::GetObstacleDataByObstacleAsset(const TSubclassOf<AObstacle> ObstacleClass)
{
	if (ObstacleClass == nullptr || !ObstacleSolverMap.Contains(ObstacleClass))
	{
		return GetRandomObstacleData();
	}
	return GetObstacleData({ObstacleSolverMap[ObstacleClass]});
}

FObstacleData* ADungeonGenerator::GetObstacleData(TArray<FObstacleData*> PossibleObstacleData) const
{
	// Choose Best Low Effort Obstacle Solver
	FObstacleData* BestObstacleData = nullptr;
	bool bPlayerHasPickupItem = true;
	int PlayerNeededRunes = 2137;
	for (FObstacleData* ObstacleData: PossibleObstacleData)
	{
		// Prioritize Obstacle Solver with SpellCast!
		// Check if Player has needed runes
		TArray<URune*> MissingRunes;
		if (ObstacleData->RequiredSpellCast != nullptr &&
			!PlayerSpellBook->CanSpellBeCasted(ObstacleData->RequiredSpellCast, MissingRunes)
			)
		{
			if (MissingRunes.Num() != 0 && MissingRunes.Num() < PlayerNeededRunes)
			{
				PlayerNeededRunes = MissingRunes.Num();
				BestObstacleData = ObstacleData;
				continue;
			}
		}
		
		// Check if Player has pickup item TODO: Fix this shit
		// if (ObstacleData->RequiredPickup != nullptr && PlayerBag->GetItemByClass(ObstacleData->RequiredPickup) == nullptr)
		// {
		// 	BestObstacleData = ObstacleData;
		// }
	}

	// If no obstacle solver was found, choose random one
	if (BestObstacleData == nullptr && PossibleObstacleData.Num() > 0)
	{
		BestObstacleData = PossibleObstacleData[FMath::RandRange(0, PossibleObstacleData.Num()-1)];
	}
	
	return BestObstacleData;
}

