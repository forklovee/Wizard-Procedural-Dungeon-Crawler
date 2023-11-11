// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PropGrabInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPropGrabInterface : public UInterface
{
	GENERATED_BODY()
};

class PROCDUNGEONCRAWLER_API IPropGrabInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	void Grab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	UPrimitiveComponent* GetGrabComponent();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	void Release();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	bool CanBeGrabbed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grab")
	void SetSimulatePhysics(const bool bNewSimulatePhysics);
};
