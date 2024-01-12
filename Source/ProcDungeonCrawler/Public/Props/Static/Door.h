// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChanged, bool, bNewOpenState);

UCLASS()
class PROCDUNGEONCRAWLER_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnDoorStateChanged OnDoorStateChanged;
	
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION()
	void SetOpen(bool bNewIsOpen);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta=(AllowPrivateAccess = "true"))
	bool bInitAsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta=(AllowPrivateAccess = "true"))
	bool bIsLocked = false;
	
	bool bIsOpen = false;
};
