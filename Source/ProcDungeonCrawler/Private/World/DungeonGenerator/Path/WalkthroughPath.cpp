// Fill out your copyright notice in the Description page of Project Settings.


#include "World/DungeonGenerator/Path/WalkthroughPath.h"

AWalkthroughPath::AWalkthroughPath()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(FName("SplineComponent"));
	RootComponent = SplineComponent;

	SplineComponent->bShouldVisualizeScale = true;
	SplineComponent->ScaleVisualizationWidth = 100.f;
}

void AWalkthroughPath::BeginPlay()
{
	Super::BeginPlay();

	SplineComponent->bShouldVisualizeScale = true;
	SplineComponent->ScaleVisualizationWidth = 100.f;
	
	Tags.Add(FName("WalkthroughPath"));
}

