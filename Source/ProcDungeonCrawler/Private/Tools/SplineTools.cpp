// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/SplineTools.h"

#include "Components/SplineComponent.h"

USplineTools::USplineTools(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool USplineTools::IsSplineIntersect(const UWorld* World,
									 const FVector& FirstSplineLocation, const USplineComponent* FirstSpline,
                                     const FVector& SecondSplineLocation, const USplineComponent* SecondSpline,
                                     FVector& IntersectionPoint,
                                     const bool bDebug)
{
	const FVector DebugSplineHandleSize = FVector::OneVector*.5f;
	
	const FVector FirstStart = FirstSplineLocation + FirstSpline->GetSplinePointAt(1, ESplineCoordinateSpace::Local).Position;
	const FVector FirstEnd = FirstSplineLocation + FirstSpline->GetSplinePointAt(0, ESplineCoordinateSpace::Local).Position;
	
	const FVector SecondStart = SecondSplineLocation + SecondSpline->GetSplinePointAt(1, ESplineCoordinateSpace::Local).Position;
	const FVector SecondEnd = SecondSplineLocation + SecondSpline->GetSplinePointAt(0, ESplineCoordinateSpace::Local).Position;

	if (bDebug)
	{
		// First Spline Debug
		DrawDebugBox(World, FirstStart, DebugSplineHandleSize, FColor::Orange, false, 0.1f, 0, 5.f);
		DrawDebugBox(World, FirstEnd, DebugSplineHandleSize, FColor::Orange, false, 0.1f, 0, 5.f);

		// Second Spline Debug
		DrawDebugBox(World, SecondStart, DebugSplineHandleSize, FColor::Yellow, false, 0.1f, 0, 5.f);
		DrawDebugBox(World, SecondEnd, DebugSplineHandleSize, FColor::Yellow, false, 0.1f, 0, 5.f);
	}
	
	const float Top = (SecondEnd.X - SecondStart.X)*(FirstStart.Y - SecondStart.Y) - (SecondEnd.Y - SecondStart.Y)*(FirstStart.X - SecondStart.X);
	const float UTop = (SecondStart.Y - FirstStart.Y)*(FirstStart.X - FirstEnd.X) - (SecondStart.X - FirstStart.X)*(FirstStart.Y - FirstEnd.Y);
	const float Bottom = (SecondEnd.Y - SecondStart.Y)*(FirstEnd.X-FirstStart.X) - (SecondEnd.X - SecondStart.X)*(FirstEnd.Y - FirstStart.Y);
	const float T = Top/Bottom;
	const float U = UTop/Bottom;
	
	IntersectionPoint = FVector(
		FMath::Lerp(FirstStart.X, FirstEnd.X, T),
		FMath::Lerp(FirstStart.Y, FirstEnd.Y, T),
		0.f
		);
	
	if (T >= 0.0 && T <= 1.0
		&& U >= 0.0 && U <= 1.0
		&& Bottom != 0.0) // Valid intersection
	{
		if (bDebug)
		{
			// Valid Intersection Debug
			DrawDebugBox(World, IntersectionPoint, DebugSplineHandleSize, FColor::Green, false, 0.1f, 0, 10.f);
		}
		return true;
	}

	if (bDebug)
	{
		// Invalid Intersection Debug
		DrawDebugBox(World, IntersectionPoint, DebugSplineHandleSize, FColor::Red, false, 0.1f, 0, 10.f);
	}
	
	return false;
	
}
