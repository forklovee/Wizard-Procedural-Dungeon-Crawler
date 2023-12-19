// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightSocket.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API ALightSocket : public AActor
{
	GENERATED_BODY()
	
public:	
	ALightSocket();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
