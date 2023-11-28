// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "RoomDoorMarker.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROCDUNGEONCRAWLER_API UDoorMarkerVolume : public UBoxComponent
{
	GENERATED_BODY()
	
public:	
	UDoorMarkerVolume();

protected:
	virtual void BeginPlay() override;
};
