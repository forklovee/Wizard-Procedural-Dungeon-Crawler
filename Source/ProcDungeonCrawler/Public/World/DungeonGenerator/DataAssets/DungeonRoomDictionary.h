// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonRuleDictionary.h"
#include "Engine/DataAsset.h"
#include "DungeonRoomDictionary.generated.h"

class ARoomPCGGlobalVolume;
class UPCGGraph;
class ADungeonRoom;
enum class ERoomType : uint8;

USTRUCT(BlueprintType)
struct FRoomResourceEntry
{
	GENERATED_BODY()

	FRoomResourceEntry()
	{
		RoomClass = nullptr;
		PCGRoomVolumeClass = nullptr;
		Probability = 1.f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADungeonRoom> RoomClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ARoomPCGGlobalVolume> PCGRoomVolumeClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Probability = 1.f;
};

USTRUCT(BlueprintType)
struct FRoomRepositoryEntry
{
	GENERATED_BODY()

	FRoomRepositoryEntry()
	{
		PossibleRooms = {};
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< FRoomResourceEntry > PossibleRooms;
};

UCLASS()
class PROCDUNGEONCRAWLER_API UDungeonRoomDictionary : public UDataAsset
{
	GENERATED_BODY()

public:
	bool HasRoomClassOfType(const ERoomType RoomType) const;
	FRoomResourceEntry GetRandomRoomClassOfType(const ERoomType RoomType);

private:
	int GetRandomRoomClassIndexByWeight(TArray<int>& Weights) const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ERoomType, FRoomRepositoryEntry> RoomRepository;
};
