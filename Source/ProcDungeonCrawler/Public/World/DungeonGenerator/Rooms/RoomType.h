// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RoomType.generated.h"

struct FObstacleData;
struct FRoomPossibleAssetType;

UENUM(BlueprintType)
enum class ERoomType: uint8
{
	None,
	
	Start,
	Corridor,
	Stairs,
	End,

	Basic,
	
	LootRoom,

	MiniBoss,
	MainBoss
};
UCLASS(BlueprintType, Blueprintable)
class PROCDUNGEONCRAWLER_API URoomType : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERoomType RoomType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasObstacle = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bHasObstacle"))
	TArray<FObstacleData> PossibleObstacleTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FRoomPossibleAssetType> PossibleAssetTypes;
};
