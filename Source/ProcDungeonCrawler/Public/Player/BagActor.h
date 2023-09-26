// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BagActor.generated.h"

class UBagUI;
class UWidgetComponent;

UCLASS()
class PROCDUNGEONCRAWLER_API ABagActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABagActor();
	
	void ToggleBag();
	bool IsOpen() const { return bIsOpen; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBagUI> BagUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BagMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* BagWidget;
	
private:
	bool bIsOpen = false;
};
