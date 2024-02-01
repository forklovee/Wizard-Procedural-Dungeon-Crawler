// Fill out your copyright notice in the Description page of Project Settings.


#include "World/DungeonGenerator/Rooms/CorridorRoom.h"
#include "Components/SplineComponent.h"

ACorridorRoom::ACorridorRoom()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACorridorRoom::BeginPlay()
{
	GenerateRoomSpline(FVector::ForwardVector);
	
	Super::BeginPlay();
}

void ACorridorRoom::GenerateRoomSpline(FVector BuildDirection)
{
	BuildDirection.Normalize();
	
	FVector CenterPoint = FVector::ZeroVector;
	TArray<TArray<FVector>> PointPairs;
	for (int PointPairId = 0; PointPairId < 3; PointPairId++)
	{
		// Get width points
		FVector RightVector = FVector::CrossProduct(BuildDirection, FVector::UpVector) * CorridorWidth * .5f;
		
		TArray<FVector> PointPair = {
			CenterPoint - RightVector, // Left Point
			CenterPoint + RightVector// Right Point
			};
		PointPairs.Add(PointPair);
		
		if (PointPairId == 1 && bBendEnabled) // Set Bend Direction
		{
			TArray<float> PossibleAngles;
			for (TTuple<float, bool>& BendAngle : BendAngles)
			{
				if (BendAngle.Value)
				{
					PossibleAngles.Add(BendAngle.Key);
				}
			}
			
			const float BendAngle = PossibleAngles[FMath::RandRange(0, PossibleAngles.Num() - 1)];
			BuildDirection = BuildDirection.RotateAngleAxis(BendAngle, FVector::UpVector);
			UE_LOG(LogTemp, Display, TEXT("BendAngle: %f"), BendAngle);

			const int PointToRaiseId = BendAngle > 0.f ? 1 : 0;
			FVector& OuterBendPoint = PointPairs[1][PointToRaiseId];
			FVector& InnerBendPoint = PointPairs[1][PointToRaiseId == 0 ? 1 : 0];

			// Calculate bend center corner
			RotatePointAroundCenter(OuterBendPoint, InnerBendPoint, BendAngle*.5f);
			FVector DirectionToBendCenterCorner = (InnerBendPoint - OuterBendPoint).GetSafeNormal();
			const float BetaAngleRad = FMath::DegreesToRadians(90.f - BendAngle*.5f);
			OuterBendPoint = InnerBendPoint - DirectionToBendCenterCorner * (CorridorWidth / FMath::Sin(BetaAngleRad));
			
			RotatePointAroundCenter(CenterPoint, InnerBendPoint, BendAngle);
			
			DrawDebugBox(GetWorld(), GetActorLocation() + OuterBendPoint, FVector::OneVector * 10.f, FColor::Green, true, 100.f, 4, 5.f);
			DrawDebugBox(GetWorld(), GetActorLocation() + CenterPoint, FVector::OneVector * 10.f, FColor::Red, true, 100.f, 4, 5.f);
		}

		const float Ratio = PointPairId == 1 ? BendRatio : 1.f - BendRatio;
		CenterPoint += BuildDirection * MinCorridorLength * Ratio;
	}

	// Set spline points 
	RoomBuildSpline->ClearSplinePoints();

	for (int PointId = 1; PointId >= 0; PointId--)
	{
		for (int StartId = PointId == 0 ? PointPairs.Num()-1 : 0;
			PointId == 0 ? StartId >= 0 : StartId < PointPairs.Num();
			PointId == 0 ? StartId-- : StartId++ )
		{
			RoomBuildSpline->AddSplinePoint(PointPairs[StartId][PointId], ESplineCoordinateSpace::Local, false);
			RoomBuildSpline->SetTangentsAtSplinePoint(RoomBuildSpline->GetNumberOfSplinePoints()-1, FVector::ZeroVector, FVector::ZeroVector, ESplineCoordinateSpace::Local, false);
		}
	}
	RoomBuildSpline->SetClosedLoop(true);
}


