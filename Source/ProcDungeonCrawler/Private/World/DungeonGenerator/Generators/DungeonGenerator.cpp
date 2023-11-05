#include "World/DungeonGenerator/Generators/DungeonGenerator.h"

#include "World/DungeonGenerator/DataAssets/DungeonConfig.h"
#include "World/DungeonGenerator/DataAssets/DungeonRoomDictionary.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"

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
	if (!LoadDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}

	// Create start room to feed generation algorithm
	FGenRoomData RoomData = FGenRoomData(0, ERoomType::Start);
	Rooms.Add( RoomData );
	Rooms = GenerateDungeonTree(RoomData, Rooms);

	UE_LOG(LogTemp, Warning, TEXT("Generated rooms: %i"), Rooms.Num());
	
	return Rooms.Num() > 1;
}

bool ADungeonGenerator::BuildDungeon()
{
	if (!LoadDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}

	for (int RoomIdx = 0; RoomIdx < Rooms.Num(); RoomIdx++)
	{
		// get generated room data
		FGenRoomData& RoomData = Rooms[RoomIdx];

		//debug
		UE_LOG(LogTemp, Warning, TEXT("Room %i: %s"), RoomData.Id, *UEnum::GetValueAsString(RoomData.RoomType));

		// get room class to spawn a premade room actor
		TSubclassOf<ADungeonRoom> RoomClass = DungeonRoomDictionary->GetRandomRoomClassOfType(RoomData.RoomType);
		if (RoomClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Room class is null!"));
			return false;
		}

		// spawn room actor
		if (ADungeonRoom* DungeonRoom = Cast<ADungeonRoom>(GetWorld()->SpawnActor(RoomClass)))
		{
			RoomData.RoomActor = DungeonRoom;

			// Start room position doesn't get calculated
			if (RoomData.Id == 0){
				DungeonRoom->SetActorLocation(-DungeonRoom->GetRoomCenter());
				DungeonRoom->Build();
				continue;
			}
			// Start room position doesn't get calculated

			// get last room data
			FGenRoomData& LastRoomData = Rooms[RoomIdx-1];

			// get current room wall data
			TArray<FRoomWall> ThisRoomWalls = RoomData.RoomActor->GetRoomWalls();
			// get random room wall
			while (ThisRoomWalls.Num() > 0)
			{
				// const int RandomWallIndex = FMath::RandRange(0, ThisRoomWalls.Num()-1);
				const int RandomWallIndex = 0;
				UE_LOG(LogTemp, Error, TEXT("%s: RandomWallIndex: %i/%i"), *GetName(), RandomWallIndex, ThisRoomWalls.Num());
				const FRoomWall& ThisRoomWall = ThisRoomWalls[RandomWallIndex];
				const FVector ThisWallNormal = ThisRoomWall.GetWallNormal();
				// invert current room random wall normal to get last room desired wall normal
				FVector LastRoomDesiredWallNormal = -ThisWallNormal;

				TArray<FRoomWall> LastRoomWalls = LastRoomData.RoomActor->GetRoomWallsOfNormal(LastRoomDesiredWallNormal);
				if (LastRoomWalls.Num() == 0)
				{
					ThisRoomWalls.RemoveAt(RandomWallIndex);
					UE_LOG(LogTemp, Error, TEXT("No compatible walls found for new room!"));
					continue;
				}

				// compatible last room wall found!!!
				// TODO: get random last room wall -> check if its valid -> if not get another one
				FRoomWall& LastRoomWall = LastRoomWalls[0];
				
				// align current room actor to last room actor 
				FVector ThisWallStart = ThisRoomWall.StartPoint;
				FVector ThisWallEnd = ThisRoomWall.EndPoint;
				// Check point directions from center to determine which point starts the wall - to the left
				TArray<FVector> LastWallPointDirections = LastRoomWall.GetPointDirectionsFromWallCenter();
				TArray<FVector> ThisWallPointDirections = ThisRoomWall.GetPointDirectionsFromWallCenter();
				if (!LastWallPointDirections[0].Equals(ThisWallPointDirections[0], 0.1))
				{
					// invert wall start point
					ThisWallStart = ThisRoomWall.EndPoint;
					ThisWallEnd = ThisRoomWall.StartPoint;
				}

				bool IsOverlapping = false;

				// const FVector ThisRoomLocation = LastRoomData.RoomActor->GetActorLocation() + LastRoomWall.StartPoint - ThisWallStart;
				const FVector ThisRoomLocation = LastRoomData.RoomActor->GetActorLocation() + LastRoomData.Id*FVector(50, 0, 0);
				for (FRoomWall& RoomWall: LastRoomWalls)
				{
					if (DungeonRoom->IsOverlappingWithRoom(ThisRoomWall, LastRoomData.RoomActor.Get(), RoomWall))
					{
						IsOverlapping = true;
						break;
					}
				
				}

				// for (const FGenRoomData& Room : Rooms)
				// {
				// 	if (!Room.RoomActor.IsValid() || Room.Id == RoomData.Parent->Id) continue;
				// 	
				// 	
				// }
				if (IsOverlapping)
				{
					UE_LOG(LogTemp, Error, TEXT("Room %i is overlapping!"), RoomData.Id);
				}
				
				
				UE_LOG(LogTemp, Error, TEXT("Room %i is not overlapping at all!"), RoomData.Id);
				DungeonRoom->SetActorLocation(ThisRoomLocation);
				DungeonRoom->Build();
				break;
			}
			
			
			
			
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create a Room Actor!"));
			return false;
		}
	}
	
	return true;
}

bool ADungeonGenerator::LoadDungeonData()
{
	if (DungeonConfigDataAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("DungeonConfig is null!"));
		return false;
	}
	
	if (DungeonConfig == nullptr)
	{
		DungeonConfig = DungeonConfigDataAsset.LoadSynchronous();
	}
	if (DungeonConfig->DungeonRuleDictionaryDataAsset.IsNull() ||
		DungeonConfig->DungeonRoomDictionaryDataAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("Some Data Asset in Dungeon Config is NULL!"));
		return false;
	}

	if (DungeonRoomDictionary == nullptr)
	{
		DungeonRoomDictionary = DungeonConfig->DungeonRoomDictionaryDataAsset.LoadSynchronous();
	}
	if (!DungeonRoomDictionary->HasRoomClassOfType(ERoomType::Start))
	{
		UE_LOG(LogTemp, Error, TEXT("No Start Room in Room Dictionary!"));
		return false;
	}
	if (DungeonRuleDictionary == nullptr)
	{
		DungeonRuleDictionary = DungeonConfig->DungeonRuleDictionaryDataAsset.LoadSynchronous();
	}
	if (!DungeonRuleDictionary->HasRoomCollectionOfType(ERoomType::Start))
	{
		UE_LOG(LogTemp, Error, TEXT("No Start Room in Rule Dictionary!"));
		return false;
	}

	return true;
}

TArray<FGenRoomData> ADungeonGenerator::GenerateDungeonTree(FGenRoomData& RoomData, TArray<FGenRoomData>& RoomTree)
{
	if (!LoadDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return RoomTree;
	}
	
	const ERoomType RoomType = RoomData.RoomType;
	if (RoomType == ERoomType::None)
	{
		UE_LOG(LogTemp, Error, TEXT("Room type is None!"));
		return RoomTree;
	}
	
	if (DungeonRuleDictionary->HasRoomCollectionOfType(RoomType))
	{
		if (!RoomTree.ContainsByPredicate([=](const FGenRoomData& CheckedRoomData)
		{
			return CheckedRoomData.RoomType == RoomData.RoomType && CheckedRoomData.Id == RoomData.Id;
		}))
		{
			RoomTree.Add(RoomData);
		}
		
		FRuleCollection RuleCollection = DungeonRuleDictionary->GetRandomRoomCollectionByType(RoomType);
		for (const ERoomType& NewRoomType : RuleCollection.Rules)
		{
			FGenRoomData NewRoomData = FGenRoomData(RoomTree.Num(), NewRoomType);
			GenerateDungeonTree(NewRoomData, RoomTree);
			
			// Connect the rooms
			RoomData.Children.Add(&NewRoomData);
			NewRoomData.Parent = &RoomData;
		}
	}
	else
	{
		RoomTree.Add(RoomData);
	}
	return RoomTree;
}

