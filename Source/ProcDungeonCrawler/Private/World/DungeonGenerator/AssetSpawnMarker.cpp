// Fill out your copyright notice in the Description page of Project Settings.


#include "World/DungeonGenerator/AssetSpawnMarker.h"

// Sets default values
AAssetSpawnMarker::AAssetSpawnMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
}

// Called when the game starts or when spawned
void AAssetSpawnMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAssetSpawnMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

