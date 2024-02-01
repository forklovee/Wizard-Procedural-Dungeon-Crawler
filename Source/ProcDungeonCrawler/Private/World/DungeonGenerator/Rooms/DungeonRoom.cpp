

#include "World/DungeonGenerator/Rooms/DungeonRoom.h"

#include "Components/SplineComponent.h"
#include "Tools/SplineTools.h"

#include "PCG/Public/PCGComponent.h"

ADungeonRoom::ADungeonRoom()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	
	RoomBoundsSpline = CreateDefaultSubobject<USplineComponent>(FName("RoomBoundsSpline"));
	RoomBoundsSpline->ComponentTags.Add(FName("RoomBoundsSpline"));
	RoomBoundsSpline->SetupAttachment(RootComponent);

	RoomHeightSpline = CreateDefaultSubobject<USplineComponent>(FName("RoomHeightSpline"));
	RoomHeightSpline->ComponentTags.Add(FName("RoomHeightSpline"));
	RoomHeightSpline->SetupAttachment(RootComponent);
	
	RoomBuildSpline = CreateDefaultSubobject<USplineComponent>(FName("RoomBuildSpline"));
	RoomBuildSpline->ComponentTags.Add(FName("RoomBaseSpline"));
	RoomBuildSpline->SetupAttachment(RootComponent);

	RoomWallsPCGComponent = CreateDefaultSubobject<UPCGComponent>(FName("RoomWallsPCG"));
	RoomWallsPCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnLoad;
	
	AssetPlacerPCGComponent = CreateDefaultSubobject<UPCGComponent>(FName("AssetPlacerPCG"));
	AssetPlacerPCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand;
}

void ADungeonRoom::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateRoomWalls();
	
	Tags.Add(TargetRoomPCGTag);
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

void ADungeonRoom::GenerateRoomWalls()
{
	// Generate Room Walls and Bounds Spline
	RoomBoundsSpline->ClearSplinePoints();
	
	const int SplinePointsAmount = RoomBuildSpline->GetNumberOfSplinePoints();
	if (SplinePointsAmount < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Room Spline! - less than 2 points!"))
		return;
	}
	
	RoomBoundsSpline->SetClosedLoop(true);

	const FVector FirstBuildSplinePointPosition = RoomBuildSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	const FVector SecondBuildSplinePointPosition = RoomBuildSpline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
	const FVector FirstWallNormal = FRoomWall(FirstBuildSplinePointPosition, SecondBuildSplinePointPosition).GetWallNormal();
	RoomBoundsSpline->AddSplineLocalPoint(FirstBuildSplinePointPosition);
	RoomBoundsSpline->SetTangentsAtSplinePoint(0, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
	
	// Generate Bounds Spline
	FVector LastWallNormal = FirstWallNormal;
	for (int PointId = 1; PointId < SplinePointsAmount; PointId++)
	{
		const FVector LastBoundSplinePointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(PointId-1, ESplineCoordinateSpace::Local);
		const FVector LastBuildSplinePointPosition = RoomBuildSpline->GetLocationAtSplinePoint(PointId-1, ESplineCoordinateSpace::Local);
		const FVector ThisSplinePointPosition = RoomBuildSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);
		const FVector WallNormal = FRoomWall(LastBuildSplinePointPosition, ThisSplinePointPosition).GetWallNormal();
		
		UE_LOG(LogTemp, Display, TEXT("ID: %i, Normal: %f, %f"), PointId, WallNormal.X, WallNormal.Y)
		RoomBoundsSpline->SetLocationAtSplinePoint(PointId-1,LastBoundSplinePointPosition + WallNormal*100.f, ESplineCoordinateSpace::Local, false);
		RoomBoundsSpline->SetTangentsAtSplinePoint(PointId-1, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);

		RoomBoundsSpline->AddSplineLocalPoint(ThisSplinePointPosition + WallNormal*100.f);
		RoomBoundsSpline->SetTangentsAtSplinePoint(PointId, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
	}
	const FVector LastBuildSplinePointPosition = RoomBuildSpline->GetLocationAtSplinePoint(SplinePointsAmount-1, ESplineCoordinateSpace::Local);
	const FVector LastBoundsSplinePointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(SplinePointsAmount-1, ESplineCoordinateSpace::Local);
	const FVector FirstBoundsSplinePointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	LastWallNormal = FRoomWall(LastBuildSplinePointPosition, FirstBoundsSplinePointPosition).GetWallNormal();

	RoomBoundsSpline->SetLocationAtSplinePoint(0,FirstBoundsSplinePointPosition + LastWallNormal*100.f, ESplineCoordinateSpace::Local, false);
	RoomBoundsSpline->SetLocationAtSplinePoint(SplinePointsAmount-1,LastBoundsSplinePointPosition + LastWallNormal*100.f, ESplineCoordinateSpace::Local, false);
	RoomBoundsSpline->SetTangentsAtSplinePoint(SplinePointsAmount-1, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);

	// Generate Room Walls
	for (int PointId = 1; PointId < SplinePointsAmount; PointId++)
	{
		const FVector LastPointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(PointId-1, ESplineCoordinateSpace::Local);
		const FVector ThisPointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);
		FRoomWall NewRoomWall = FRoomWall(LastPointPosition, ThisPointPosition);

		RoomWalls.Add(NewRoomWall);
	}
	const FVector EndPointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(SplinePointsAmount-1, ESplineCoordinateSpace::Local);
	const FVector FirstPointPosition = RoomBoundsSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	FRoomWall NewRoomWall = FRoomWall(EndPointPosition, FirstPointPosition);

	RoomWalls.Add(NewRoomWall);
	
}

FVector ADungeonRoom::GetRoomCenter() const
{
	const int SplinePointsAmount = RoomBuildSpline->GetNumberOfSplinePoints();
	FVector VectorSum = FVector::ZeroVector;
	for (int PointId = 0; PointId < SplinePointsAmount; PointId++)
	{
		VectorSum += RoomBuildSpline->GetLocationAtSplinePoint(PointId, ESplineCoordinateSpace::Local);
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
			UE_LOG(LogTemp, Display, TEXT("Overlapping!"))
			return true;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Point not inside room."))
	
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

void ADungeonRoom::RotatePointAroundCenter(FVector& Point, const FVector& Center, const float AngleDeg) const
{
	const float Angle = FMath::DegreesToRadians(AngleDeg);
	Point = FVector(
	Center.X + (Point.X-Center.X)*FMath::Cos(Angle) - (Point.Y-Center.Y)*FMath::Sin(Angle),
	Center.Y + (Point.X-Center.X)*FMath::Sin(Angle) + (Point.Y-Center.Y)*FMath::Cos(Angle),
	0.f
	);
}

void ADungeonRoom::ScaleWallByPoint(FRoomWall& RoomWall, FVector Point, float SizeChange) const
{
	FVector NewStartPoint = RoomWall.StartPoint;
	NewStartPoint.X = Point.X + (RoomWall.StartPoint.X - Point.X) * SizeChange;
	NewStartPoint.Y = Point.Y + (RoomWall.StartPoint.Y - Point.Y) * SizeChange;
	
	FVector NewEndPoint = RoomWall.EndPoint;
	NewEndPoint.X = Point.X + (RoomWall.EndPoint.X - Point.X) * SizeChange;
	NewEndPoint.Y = Point.Y + (RoomWall.EndPoint.Y - Point.Y) * SizeChange;

	RoomWall.StartPoint = NewStartPoint;
	RoomWall.EndPoint = NewEndPoint;
}
