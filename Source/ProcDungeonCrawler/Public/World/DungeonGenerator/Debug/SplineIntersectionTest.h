// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineIntersectionTest.generated.h"

class USplineComponent;

UCLASS()
class PROCDUNGEONCRAWLER_API ASplineIntersectionTest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASplineIntersectionTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Test")
	bool bShowDebug = true;
	
	UPROPERTY(EditAnywhere, Category="Test")
	ASplineIntersectionTest* OtherSplineActor;
	
	UPROPERTY(EditAnywhere, Category="Components")
	USplineComponent* SplineComponent;
};
