// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SplineTools.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API USplineTools : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static bool IsSplineIntersect(const UWorld* World,
		const FVector& FirstSplineLocation, const USplineComponent* FirstSpline,
		const FVector& SecondSplineLocation, const USplineComponent* SecondSpline,
		FVector& IntersectionPoint,
		const bool bDebug = false);
};
