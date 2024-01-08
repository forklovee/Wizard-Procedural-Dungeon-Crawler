// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ShapeComponent.h"
#include "SpellInteractionShapeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCDUNGEONCRAWLER_API USpellInteractionShapeComponent : public UShapeComponent
{
	GENERATED_BODY()

public:
	USpellInteractionShapeComponent();

protected:
	virtual void BeginPlay() override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult);
};
