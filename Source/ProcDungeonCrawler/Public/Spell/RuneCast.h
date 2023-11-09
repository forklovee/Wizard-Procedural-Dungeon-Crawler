// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RuneCast.generated.h"

class AWizardCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRuneCastedSignature, AWizardCharacter*, WizardCharacter, URuneCast*, RuneCast);

UCLASS()
class PROCDUNGEONCRAWLER_API URuneCast : public UDataAsset
{
	GENERATED_BODY()

public:
	FOnRuneCastedSignature OnRuneCasted;

	UFUNCTION(BlueprintCallable, Category = "Rune")
	void CastRune(AWizardCharacter* WizardCharacter);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	FName RuneName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	TSoftObjectPtr<UTexture2D> RuneIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune")
	float ManaCost = 0.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune|Damage")
	bool bDealsDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rune|Damage", meta=(EditCondition="bDealsDamage"))
	float Damage = 0.0;
};
