// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShapeComponent.h"
#include "InteractionShapeComponent.generated.h"

class AItem;
class AHuman;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionSuccessful, AHuman*, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFailed, AHuman*, Pawn);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCDUNGEONCRAWLER_API UInteractionShapeComponent : public UShapeComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractionSuccessful OnInteractionSuccessful;
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFailed OnInteractionFailed;
	
	UInteractionShapeComponent();

	bool Interact(AHuman* Pawn);
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<TSubclassOf<AItem>> RequiredItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bConsumeRequiredItems = false;
};
