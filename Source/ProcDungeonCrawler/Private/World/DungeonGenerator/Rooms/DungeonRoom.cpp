

#include "World/DungeonGenerator/Rooms/DungeonRoom.h"

#include "Components/SplineComponent.h"
#include "Tools/SplineTools.h"

#include "PCG/Public/PCGComponent.h"

ADungeonRoom::ADungeonRoom()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	RoomBaseSpline = CreateDefaultSubobject<USplineComponent>(FName("RoomBaseSpline"));
	RoomBaseSpline->ComponentTags.Add(FName("RoomBaseSpline"));
	RoomBaseSpline->SetupAttachment(RootComponent);

	RoomWallsPCGComponent = CreateDefaultSubobject<UPCGComponent>(FName("RoomWallsPCG"));
	RoomWallsPCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnLoad;
	
	AssetPlacerPCGComponent = CreateDefaultSubobject<UPCGComponent>(FName("AssetPlacerPCG"));
	AssetPlacerPCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand;
}

void ADungeonRoom::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(TargetRoomPCGTag);
}

FVector ADungeonRoom::GetRoomCenter() const
{
	const int SplinePointsAmount = RoomBaseSpline->GetNumberOfSplinePoints();
	FVector VectorSum = FVector::ZeroVector;
	for (int PointId = 0; PointId < SplinePointsAmount; PointId++)
	{
		VectorSum += RoomBaseSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);
	}
	return VectorSum / SplinePointsAmount;
}

TArray<FRoomWall> ADungeonRoom::GetRoomWalls() const
{
	TArray<FRoomWall> RoomWalls = TArray<FRoomWall>();

	const int SplinePointsAmount = RoomBaseSpline->GetNumberOfSplinePoints();
	if (SplinePointsAmount < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Room Spline! - less than 2 points!"))
		return RoomWalls;
	}

	for (int PointId = 1; PointId < SplinePointsAmount; PointId++)
	{
		const FVector LastPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(PointId-1, ESplineCoordinateSpace::Local);
		const FVector ThisPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);
		
		RoomWalls.Add(FRoomWall(LastPointPosition, ThisPointPosition));
	}
	const FVector LastPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(SplinePointsAmount-1, ESplineCoordinateSpace::Local);
	const FVector FirstPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	RoomWalls.Add(FRoomWall(LastPointPosition, FirstPointPosition));
	
	return RoomWalls;
}

TArray<FRoomWall> ADungeonRoom::GetRoomWallsOfNormal(const FVector& WallNormal) const
{
	TArray<FRoomWall> RoomWalls = TArray<FRoomWall>();

	const int SplinePointsAmount = RoomBaseSpline->GetNumberOfSplinePoints();
	if (SplinePointsAmount < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Room Spline! - less than 2 points!"))
		return RoomWalls;
	}

	for (int PointId = 1; PointId < SplinePointsAmount; PointId++)
	{
		const FVector LastPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(PointId-1, ESplineCoordinateSpace::Local);
		const FVector ThisPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);

		FRoomWall RoomWall = FRoomWall(LastPointPosition, ThisPointPosition);
		FVector RoomWallNormal = RoomWall.GetWallNormal();
		if (RoomWallNormal.Equals(WallNormal, 0.1f))
		{
			RoomWalls.Add(RoomWall);
		}
	}
	// Connect last point to first point
	const FVector LastPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(SplinePointsAmount-1, ESplineCoordinateSpace::Local);
	const FVector FirstPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	FRoomWall RoomWall = FRoomWall(LastPointPosition, FirstPointPosition);
	FVector RoomWallNormal = RoomWall.GetWallNormal();
	if (RoomWallNormal.Equals(WallNormal, 0.1f))
	{
		RoomWalls.Add(RoomWall);
	}
	
	return RoomWalls;
}

bool ADungeonRoom::IsOverlappingWithRoom(const FRoomWall& ThisRoomWall, const ADungeonRoom* OtherRoom, const FRoomWall& OtherRoomWall) const
{
	bool bWallsOverlapping = false;
	FVector IntersectionPoint = FVector::ZeroVector;
	if (USplineTools::AreLinesIntersecting(GetWorld(),
		ThisRoomWall.StartPoint + GetActorLocation(), ThisRoomWall.EndPoint + GetActorLocation(),
		OtherRoomWall.StartPoint + OtherRoom->GetActorLocation(), OtherRoomWall.EndPoint + OtherRoom->GetActorLocation(),
		IntersectionPoint,
		bWallsOverlapping,
		true))
	{
		DrawDebugBox(GetWorld(), IntersectionPoint, FVector(10.f, 10.f, 10.f), FColor::Red, true, 100.f, 0, 10.f);
		return true;
	}

	return false;
}