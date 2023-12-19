// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/Static/LightSocket/LightSocket.h"

// Sets default values
ALightSocket::ALightSocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightSocket::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightSocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

