// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/Static/Staircase.h"

#include "Components/SplineComponent.h"

// Sets default values
AStaircase::AStaircase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaircaseSpline = CreateDefaultSubobject<USplineComponent>(FName("StaircaseSpline"));
	RootComponent = StaircaseSpline;
	StaircaseSpline->ComponentTags.Add(FName("StaircaseSpline"));
	StaircaseSpline->ComponentTags.Add(FName("Obstacle"));
}

// Called when the game starts or when spawned
void AStaircase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStaircase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

