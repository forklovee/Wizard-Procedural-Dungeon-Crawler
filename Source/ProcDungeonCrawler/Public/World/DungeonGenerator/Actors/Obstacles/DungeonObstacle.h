
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonObstacle.generated.h"

class ASpellCast;
class APickupItem;

USTRUCT()
struct FObstacleData: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADungeonObstacle> ObstacleClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RequiredTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< TSubclassOf<APickupItem> > RequiredPickups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray< TSubclassOf<ASpellCast> > RequiredSpellCasts;
};

UCLASS()
class PROCDUNGEONCRAWLER_API ADungeonObstacle : public AActor
{
	GENERATED_BODY()

public:
	ADungeonObstacle();

protected:
	virtual void BeginPlay() override;
};
