// Fill out your copyright notice in the Description page of Project Settings.


#include "World/DungeonGenerator/DataAssets/DungeonRoomDictionary.h"

bool UDungeonRoomDictionary::HasRoomClassOfType(const ERoomType RoomType) const
{
	return RoomRepository.Contains(RoomType);
}

FRoomResourceEntry UDungeonRoomDictionary::GetRandomRoomClassOfType(const ERoomType RoomType)
{
	FRoomRepositoryEntry* RoomRepositoryEntry = RoomRepository.Find(RoomType);
	if (RoomRepositoryEntry == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("RoomRepositoryEntry is NULL!"));
		return FRoomResourceEntry();
	}
	
	TArray<int> Weights;
	for (const FRoomResourceEntry& RoomResourceEntry: RoomRepositoryEntry->PossibleRooms)
	{
		Weights.Add(RoomResourceEntry.Probability);
	}
	const int RandomRoomIndex = GetRandomRoomClassIndexByWeight(Weights);
	if (RandomRoomIndex == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("Weight search of %i: RoomIndex is -1"), Weights.Num());
		return FRoomResourceEntry();
	}
	
	return RoomRepositoryEntry->PossibleRooms[RandomRoomIndex];
}

int UDungeonRoomDictionary::GetRandomRoomClassIndexByWeight(TArray<int>& Weights) const
{
	int Sum = 0;
	for (const int& Weight: Weights)
	{
		Sum += Weight;
	}
	int R = FMath::RandRange(0, Sum);
	for (int i = 0; i < Weights.Num(); ++i)
	{
		R -= Weights[i];
		if (R <= 0) //TODO: check if its correct i mean the <= part
		{
			return i;
		}
	}
	return -1;
}
