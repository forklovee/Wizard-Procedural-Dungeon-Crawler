// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "WalkthroughPath.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API AWalkthroughPath : public AActor
{
	GENERATED_BODY()
	
public:	
	AWalkthroughPath();

protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	USplineComponent* SplineComponent;
};
