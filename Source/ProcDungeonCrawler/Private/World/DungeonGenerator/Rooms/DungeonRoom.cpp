

#include "World/DungeonGenerator/Rooms/DungeonRoom.h"

#include "FrameTypes.h"
#include "PCGComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"


ADungeonRoom::ADungeonRoom()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	RoomBaseSpline = CreateDefaultSubobject<USplineComponent>(FName("RoomBaseSpline"));
	RoomBaseSpline->ComponentTags.Add(FName("RoomBaseSpline"));
	RoomBaseSpline->SetupAttachment(RootComponent);
	
	PCGComponent = CreateDefaultSubobject<UPCGComponent>(FName("PCGComponent"));
	PCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand; 
}

void ADungeonRoom::BeginPlay()
{
	Super::BeginPlay();
}



void ADungeonRoom::Build() const
{
	PCGComponent->Cleanup();
	PCGComponent->Generate();
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
	FVector ThisStart = ThisRoomWall.StartPoint + GetActorLocation();
	FVector ThisEnd = ThisRoomWall.EndPoint + GetActorLocation();
	
	FVector OtherStart = OtherRoomWall.StartPoint + OtherRoom->GetActorLocation();
	FVector OtherEnd = OtherRoomWall.EndPoint + OtherRoom->GetActorLocation();
	const float Top = (OtherEnd.X-OtherStart.X)*(ThisStart.Y-OtherStart.X) - (OtherEnd.Y-OtherStart.Y)*(ThisStart.X-OtherStart.X);
	const float Bottom = (OtherEnd.Y-OtherStart.Y)*(ThisEnd.X-ThisStart.X) - (OtherEnd.X-OtherStart.X)*(ThisEnd.Y-ThisStart.Y);
	const float T = Top/Bottom;

	FVector IntersectionPoint = FVector(
		FMath::Lerp(ThisStart.X, ThisEnd.X, T),
		FMath::Lerp(ThisStart.Y, ThisEnd.Y, T),
		0.f
		);

	
	
	if (T >= 0.f && T <= 1.f) // intersection
	{
		DrawDebugBox(GetWorld(), IntersectionPoint, FVector(10.f, 10.f, 10.f), FColor::Red, true, 10.f, 0, 10.f);

		return true;
	}
	return false;
}

bool ADungeonRoom::IsPointIntersectsWall(const FVector& Point, const FRoomWall& RoomWall) const
{
	const FVector StartPoint = RoomWall.StartPoint + GetActorLocation();
	const FVector EndPoint = RoomWall.EndPoint + GetActorLocation();
	const float StartPointToPointDistance = FVector::Dist(StartPoint, Point);
	const float EndPointToPointDistance = FVector::Dist(Point, EndPoint);
	
	return (StartPointToPointDistance > 0.0 || EndPointToPointDistance > 0.0) &&
		FVector::Dist(StartPoint, Point) + FVector::Dist(Point, EndPoint) == RoomWall.GetWallLength();
}