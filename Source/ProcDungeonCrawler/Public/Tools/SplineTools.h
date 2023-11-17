// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SplineTools.generated.h"

struct FSplinePoint;
class USplineComponent;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API USplineTools : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static void SetTangentsToZero(USplineComponent* SplineComponent, int PointIndex);
	
	static bool AreLinesIntersecting(const UWorld* World,
									const FVector& FirstLineStart, const FVector& FirstLineEnd,
									const FVector& SecondLineStart, const FVector& SecondLineEnd,
									FVector& IntersectionPoint,
									bool& bOverlapping,
									const bool bDebug);
	
	static bool AreSplinesIntersecting(const UWorld* World,
									const FVector& FirstSplineLocation, const USplineComponent* FirstSpline,
									const FVector& SecondSplineLocation, const USplineComponent* SecondSpline,
									FVector& IntersectionPoint,
									bool& bOverlapping,
									const bool bDebug = false);
};
