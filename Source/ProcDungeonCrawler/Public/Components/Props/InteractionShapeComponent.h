// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShapeComponent.h"
#include "InteractionShapeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, APawn*, Pawn);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCDUNGEONCRAWLER_API UInteractionShapeComponent : public UShapeComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;
	
	UInteractionShapeComponent();

	bool Interact(APawn* Pawn);
	
protected:
	virtual void BeginPlay() override;
};
