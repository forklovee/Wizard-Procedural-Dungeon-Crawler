#include "World/DungeonGenerator/Generators/DungeonGenerator.h"

#include "PCGComponent.h"
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
	if (!LoadDungeonData())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Dungeon Data!"));
		return false;
	}

	// Create start room to feed generation algorithm

	int LevelsToGenerate = DungeonConfig->Level
	

	
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
	if (WalkthroughPathClass == nullptr)
	{
		return false;
	}

	AWalkthroughPath* MainWalkthroughPath = GetWorld()->SpawnActor<AWalkthroughPath>(WalkthroughPathClass);
	for (int RoomIdx = 0; RoomIdx < Rooms.Num(); RoomIdx++)
	{
		// get generated room data
		FGenRoomData& RoomData = Rooms[RoomIdx];

		//debug
		UE_LOG(LogTemp, Warning, TEXT("Room %i: %s"), RoomData.Id, *UEnum::GetValueAsString(RoomData.RoomType));

		// get room class to spawn a premade room actor
		FRoomResourceEntry RoomResource = DungeonRoomDictionary->GetRandomRoomClassOfType(RoomData.RoomType);
		if (RoomResource.RoomClass == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Room class is null!"));
			return false;
		}

		ADungeonRoom* DungeonRoom = Cast<ADungeonRoom>(GetWorld()->SpawnActor(RoomResource.RoomClass));
		if (DungeonRoom == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Dungeon room!"));
			return false;
		}
		
		if (RoomResource.PCGRoomVolumeClass != nullptr)
		{
			if (!PCGRoomVolumes.Contains(RoomResource.PCGRoomVolumeClass))
			{
				PCGRoomVolumes.Add(RoomResource.PCGRoomVolumeClass);
			}
			FString VolumeStringName = RoomResource.PCGRoomVolumeClass->GetName();
			DungeonRoom->Tags.Add(FName(VolumeStringName));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("VOLUME CLASS NULL!!!"));
		}
		
		RoomData.RoomActor = DungeonRoom;

		// Start room position isn't calculated
		if (RoomData.Id == 0){
			DungeonRoom->SetActorLocation(-DungeonRoom->GetRoomCenter());
			continue;
		}

		// get last room data
		FGenRoomData& LastRoomData = Rooms[RoomIdx-1];

		// get current room walls data
		TArray<FRoomWall> ThisRoomWalls = RoomData.RoomActor->GetRoomWalls();

		bool bCorrectLocationFound = false;
		while (ThisRoomWalls.Num() > 0)
		{
			if (bCorrectLocationFound)
			{
				break;
			}
			const int RandomWallIndex = FMath::RandRange(0, ThisRoomWalls.Num()-1);
			UE_LOG(LogTemp, Display, TEXT("\t Room %i Selected Random Wall of Idx %i out of %i"), RoomIdx, RandomWallIndex, ThisRoomWalls.Num());

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

			
			for (FRoomWall& LastRoomWall: LastRoomWalls)
			{
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
				
				FVector ThisRoomLocation = LastRoomData.RoomActor->GetActorLocation() + LastRoomWall.StartPoint - ThisWallStart;
				ThisRoomLocation -= LastRoomDesiredWallNormal * 200.f;
				DungeonRoom->SetActorLocation(ThisRoomLocation);
				// Check this wall intersections with last room walls
				bool bIsOverlapping = false;
				for (FRoomWall& TestLastRoomWall: LastRoomData.RoomActor->GetRoomWalls())
				{
					if (DungeonRoom->IsOverlappingWithRoom(ThisRoomWall, LastRoomData.RoomActor.Get(), LastRoomWall))
					{
						UE_LOG(LogTemp, Error, TEXT("Room %i is overlapping!"), RoomData.Id);
						bIsOverlapping = true;
						break;
					}
				}

				if (bIsOverlapping)
				{
					continue;
				}
				
				MainWalkthroughPath->SplineComponent->AddSplinePoint(LastRoomData.RoomActor->GetActorLocation() + LastRoomWall.GetWallCenter(), ESplineCoordinateSpace::Local, true);
				USplineTools::SetTangentsToZero(MainWalkthroughPath->SplineComponent, MainWalkthroughPath->SplineComponent->GetNumberOfSplinePoints()-1);
				MainWalkthroughPath->SplineComponent->AddSplinePoint(DungeonRoom->GetActorLocation() + ThisRoomWall.GetWallCenter(), ESplineCoordinateSpace::Local, true);
				USplineTools::SetTangentsToZero(MainWalkthroughPath->SplineComponent, MainWalkthroughPath->SplineComponent->GetNumberOfSplinePoints()-1);
				MainWalkthroughPath->SplineComponent->AddSplinePoint(DungeonRoom->GetActorLocation() + DungeonRoom->GetRoomCenter(), ESplineCoordinateSpace::Local, true);
				USplineTools::SetTangentsToZero(MainWalkthroughPath->SplineComponent, MainWalkthroughPath->SplineComponent->GetNumberOfSplinePoints()-1);

				// good!
				bCorrectLocationFound = true;
				break;
			}
		}
	}
	
	for (TSubclassOf<ARoomPCGGlobalVolume> PcgVolume: PCGRoomVolumes)
	{
		if (PcgVolume == nullptr)
		{
			UE_LOG(LogTemp, Display, TEXT("Volume null!"))
			continue;
		}
		
		if (ARoomPCGGlobalVolume* RoomPcgRoom = GetWorld()->SpawnActor<ARoomPCGGlobalVolume>(PcgVolume))
		{
			RoomPcgRoom->SetActorScale3D(FVector3d(1000.f, 1000.f, 1000.f));
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

