// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "World/DungeonGenerator/Rooms/DungeonRoom.h"
#include "CorridorRoom.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API ACorridorRoom : public ADungeonRoom
{
	GENERATED_BODY()

public:
	ACorridorRoom();

	UFUNCTION(BlueprintCallable, Category="Room|Corridor")
	void GenerateRoomSpline(FVector BuildDirection);
	
protected:
	virtual void BeginPlay() override;

	void RotatePointAroundCenter(FVector& Point, const FVector& Center, const float AngleDeg) const;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Room|Corridor")
	int ConnectedRooms = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Corridor")
	float MinCorridorLength = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Corridor")
	float CorridorWidth = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Corridor|Bend")
	bool bBendEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Corridor|Bend")
	float BendRatio = .5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room|Corridor|Bend")
	TMap<float, bool> BendAngles = {
		{-90.f, true},
		{-45.f, true},
		{45.f, true},
		{90.f, true}
	};

	
	
};
