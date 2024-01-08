// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Clothes.h"
#include "RuneRing.generated.h"

class URune;

UCLASS()
class PROCDUNGEONCRAWLER_API ARuneRing : public AClothes
{
	GENERATED_BODY()
	
public:
	ARuneRing();
	TSoftObjectPtr<URune>& GetRuneCast();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	TSoftObjectPtr<URune> RuneCast;
};
