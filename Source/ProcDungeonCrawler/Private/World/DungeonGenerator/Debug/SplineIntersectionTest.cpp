#include "World/DungeonGenerator/Debug/SplineIntersectionTest.h"

#include "Tools/SplineTools.h"
#include "Components/SplineComponent.h"


ASplineIntersectionTest::ASplineIntersectionTest()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1.f);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(FName("Spline"));
	RootComponent = SplineComponent;
}

void ASplineIntersectionTest::BeginPlay()
{
	Super::BeginPlay();
}

void ASplineIntersectionTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OtherSplineActor == nullptr)
	{
		return;
	}
	bool bOverlapping = false;
	FVector IntersectionPoint = FVector::ZeroVector;
	if (USplineTools::AreSplinesIntersecting(GetWorld(),
		GetActorLocation(), SplineComponent,
		OtherSplineActor->GetActorLocation(), OtherSplineActor->SplineComponent,
		IntersectionPoint, bOverlapping, true))
	{
		UE_LOG(LogTemp, Display, TEXT("Intersection Point: %s"), *IntersectionPoint.ToString());
	}
}

