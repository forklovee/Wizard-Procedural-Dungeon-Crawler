// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DungeonConfig.generated.h"

class UDungeonRuleDictionary;
class UDungeonRoomDictionary;

/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UDungeonConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataAssets")
	TSoftObjectPtr<UDungeonRuleDictionary> DungeonRuleDictionaryDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DataAssets")
	TSoftObjectPtr<UDungeonRoomDictionary> DungeonRoomDictionaryDataAsset;
};
