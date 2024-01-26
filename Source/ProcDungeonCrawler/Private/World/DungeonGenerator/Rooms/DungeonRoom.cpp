

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

void ADungeonRoom::DrawDebugShapes()
{
	for (FRoomWall& RoomWall: RoomWalls)
	{
		const FVector WallCenter = GetActorLocation() + RoomWall.GetWallCenter();
		const FVector WallStartPoint = GetActorLocation() + RoomWall.StartPoint;
		const FVector WallEndPoint = GetActorLocation() + RoomWall.EndPoint;

		DrawDebugCapsule(GetWorld(),
			WallStartPoint,
			0.f, 5.f, FQuat::Identity, FColor::Red, true, 100.f, 4, 10.f);
		
		DrawDebugLine(GetWorld(),
			WallStartPoint,
			WallEndPoint,
			FColor::White, true, 100.f, 4, 10.f);
		
		DrawDebugDirectionalArrow(GetWorld(),
			WallCenter,
			GetActorLocation() + RoomWall.GetWallCenter() + RoomWall.GetWallNormal() * 100.f,
			100.f, FColor::Blue, true, 100.f, 4, 15.f);
	}
}

void ADungeonRoom::BeginPlay()
{
	Super::BeginPlay();

	GenerateRoomWalls();
	
	Tags.Add(TargetRoomPCGTag);
}

void ADungeonRoom::GenerateRoomWalls()
{
	const int SplinePointsAmount = RoomBaseSpline->GetNumberOfSplinePoints();
	if (SplinePointsAmount < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Room Spline! - less than 2 points!"))
		return;
	}

	for (int PointId = 1; PointId < SplinePointsAmount; PointId++)
	{
		const FVector LastPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(PointId-1, ESplineCoordinateSpace::Local);
		const FVector ThisPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);
		FRoomWall NewRoomWall = FRoomWall(LastPointPosition, ThisPointPosition);
		RoomWalls.Add(NewRoomWall);
	}
	
	const FVector LastPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(SplinePointsAmount-1, ESplineCoordinateSpace::Local);
	const FVector FirstPointPosition = RoomBaseSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	FRoomWall NewRoomWall = FRoomWall(LastPointPosition, FirstPointPosition);
	RoomWalls.Add(NewRoomWall);
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

TArray<FRoomWall>& ADungeonRoom::GetRoomWalls()
{
	return RoomWalls;
}

TArray<FRoomWall*> ADungeonRoom::GetValidRoomWalls()
{
	TArray<FRoomWall*> ValidRoomWalls;
	for (FRoomWall& RoomWall: RoomWalls)
	{
		//todo: implement wall range check
		ValidRoomWalls.Add(&RoomWall);
	}
	return ValidRoomWalls;
}

TArray<FRoomWall*> ADungeonRoom::GetRoomWallsOfNormal(const FVector& WallNormal)
{
	TArray<FRoomWall*> RoomWallsOfNormal;
	for (FRoomWall& RoomWall: RoomWalls)
	{
		FVector RoomWallNormal = RoomWall.GetWallNormal();
		if (RoomWallNormal.Equals(WallNormal, 0.1f))
		{
			RoomWallsOfNormal.Add(&RoomWall);
		}
	}
	return RoomWallsOfNormal;
}

TArray<FVector> ADungeonRoom::GetRoomLocalPoints()
{
	TArray<FVector> LocalPoints;
	for (FRoomWall& RoomWall: RoomWalls)
	{
		LocalPoints.Add(RoomWall.StartPoint);
	}
	return LocalPoints;
}

bool ADungeonRoom::IsPointInsideRoom(const FVector& Point)
{
	for (FRoomWall& RoomWall: RoomWalls)
	{
		const FVector StartPoint = RoomWall.StartPoint + GetActorLocation();
		const FVector EndPoint = RoomWall.EndPoint + GetActorLocation();
		if (FVector::Distance(StartPoint, Point) < 5.f || FVector::Distance(EndPoint, Point) < 5.f)
		{
			continue;
		}
		
		const FVector WallNormal = RoomWall.GetWallNormal();
		const FVector WallCenter = GetActorLocation() + RoomWall.GetWallCenter();
		const FVector2D WallNormal2D = FVector2D(WallNormal.X, WallNormal.Y);
		FVector2D DirectionToPoint2D = FVector2D(Point.X - WallCenter.X, Point.Y - WallCenter.Y).GetSafeNormal();
		const float DotProduct = FVector2D::DotProduct(WallNormal2D, DirectionToPoint2D);

		// DrawDebugDirectionalArrow(GetWorld(),
		// 	WallCenter,
		// 	WallCenter + FVector(DirectionToPoint2D.X, DirectionToPoint2D.Y, 0.f) * 100.f,
		// 	100.f, FColor::Red, true, 100.f, 4, 15.f);
		// UE_LOG(LogTemp, Display, TEXT("DotProduct: %f"), DotProduct);
		
		if (DotProduct < 0.f)
		{
			continue;
		}
		return true;
	}
	
	return false;
}

bool ADungeonRoom::IsOverlappingWithRoom(ADungeonRoom* OtherRoom)
{
	// Check if OtherRoom's corners are inside this room
	TArray<FVector> OtherRoomLocalPoints = OtherRoom->GetRoomLocalPoints();
	for (FVector& OtherRoomLocalPoint: OtherRoomLocalPoints)
	{
		if (IsPointInsideRoom(OtherRoom->GetActorLocation() + OtherRoomLocalPoint))
		{
			return true;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Point not inside room."))
	
	// OtherRoom's corners are not inside this room, check if any of the walls are overlapping
	for (FRoomWall& ThisRoomWall: RoomWalls)
	{
		const FVector ThisStartPoint = ThisRoomWall.StartPoint + GetActorLocation();
		const FVector ThisEndPoint = ThisRoomWall.EndPoint + GetActorLocation();
		for (FRoomWall& OtherRoomWall: OtherRoom->GetRoomWalls())
		{
			FVector OtherStartPoint = OtherRoomWall.StartPoint + OtherRoom->GetActorLocation();
			FVector OtherEndPoint = OtherRoomWall.EndPoint + OtherRoom->GetActorLocation();
			
			bool bWallsOverlapping = false;
			FVector IntersectionPoint = FVector::ZeroVector;
			if (USplineTools::AreLinesIntersecting(GetWorld(),
				ThisStartPoint, ThisEndPoint,
				OtherStartPoint, OtherEndPoint,
				IntersectionPoint,
				bWallsOverlapping,
				true))
			{
				DrawDebugCapsule(GetWorld(), IntersectionPoint, 0.f, 10.f, FQuat::Identity, FColor::Red, true, 100.f, 4, 10.f);
				return true;
			}
		}
	}
	return false;
}
