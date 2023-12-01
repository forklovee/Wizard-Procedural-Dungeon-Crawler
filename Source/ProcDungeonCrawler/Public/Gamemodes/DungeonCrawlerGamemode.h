// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DungeonCrawlerGamemode.generated.h"

class USpellbookComponent;
class UBagComponent;
class URuneCast;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API ADungeonCrawlerGamemode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	TWeakObjectPtr<UBagComponent> PlayerBag;
	TWeakObjectPtr<USpellbookComponent> PlayerSpellBook;
};
