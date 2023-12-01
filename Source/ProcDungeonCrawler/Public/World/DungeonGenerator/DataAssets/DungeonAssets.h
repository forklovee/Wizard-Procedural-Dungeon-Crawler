#pragma once

#include "CoreMinimal.h"

#include "DungeonAssets.generated.h"

UENUM(BlueprintType)
enum class EAssetType: uint8
{
	Asset,
	Weapon,
	Food,
	Rune
};

UENUM(BlueprintType)
enum class EAssetRarityTypeHandling: uint8
{
	OnlyTargetRarity,
	BelowTargetRarity,
	BelowEqualTargetRarity,
};

UENUM(BlueprintType)
enum class EAssetAmountHandling: uint8
{
	ExactTarget,
	MinimumTarget,
	MaximumTarget,
};

UENUM()
enum class EEnemySpawnRuleType
{
	FromClasses,
	FromRarity
};

USTRUCT(BlueprintType)
struct FRoomPossibleAssetType
{
	GENERATED_BODY()
	FRoomPossibleAssetType()
	{
		bRequired = false;
		AssetType = EAssetType::Asset;
		RarityTypeHandling = EAssetRarityTypeHandling::OnlyTargetRarity;
		TargetRarity = 0;
		AmountHandling = EAssetAmountHandling::ExactTarget;
		TargetAmount = 0;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequired = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAssetType AssetType = EAssetType::Asset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAssetRarityTypeHandling RarityTypeHandling = EAssetRarityTypeHandling::OnlyTargetRarity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TargetRarity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAssetAmountHandling AmountHandling = EAssetAmountHandling::ExactTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TargetAmount = 1;
};

USTRUCT(BlueprintType)
struct FRoomPossibleEnemyType
{
	GENERATED_BODY()
	FRoomPossibleEnemyType()
	{
		EnemySpawnRuleType = EEnemySpawnRuleType::FromClasses;
		RarityHandling = EAssetRarityTypeHandling::OnlyTargetRarity;
		TargetRarity = 0;
		TargetAmount = 1;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEnemySpawnRuleType EnemySpawnRuleType = EEnemySpawnRuleType::FromClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EnemySpawnRuleType == EEnemySpawnRuleType::FromClasses"))
	TArray<TSubclassOf<AActor>> EnemyClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EnemySpawnRuleType == EEnemySpawnRuleType::FromRarity"))
	EAssetRarityTypeHandling RarityHandling = EAssetRarityTypeHandling::OnlyTargetRarity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="EnemySpawnRuleType == EEnemySpawnRuleType::FromRarity"))
	int TargetRarity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAssetAmountHandling AmountHandling = EAssetAmountHandling::ExactTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TargetAmount = 1;
};

USTRUCT(BlueprintType)
struct FDungeonAssetResource : public FTableRowBase
{
	GENERATED_BODY()

	FDungeonAssetResource()
	{
		AssetClass = nullptr;
		AssetType = EAssetType::Asset;
		Rarity = 0;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> AssetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAssetType AssetType = EAssetType::Asset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Rarity = 0;
};

USTRUCT(BlueprintType)
struct FObstacleData: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ObstacleClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SolverRequiredTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< TSubclassOf<AActor> > RequiredPickups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< TSubclassOf<AActor> > RequiredSpellCasts;
};
	
