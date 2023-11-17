// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomPCGGlobalVolume.generated.h"

class UPCGComponent;

UCLASS(Blueprintable, BlueprintType)
class PROCDUNGEONCRAWLER_API ARoomPCGGlobalVolume : public AVolume
{
	GENERATED_BODY()
	
public:	
	ARoomPCGGlobalVolume();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG")
	FName TargetRoomsTag = FName("Basic");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG")
	UPCGComponent* PCGComponent;
};
