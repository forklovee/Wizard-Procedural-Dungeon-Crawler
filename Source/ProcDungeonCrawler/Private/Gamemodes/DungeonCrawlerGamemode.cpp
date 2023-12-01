// Fill out your copyright notice in the Description page of Project Settings.

#include "Gamemodes/DungeonCrawlerGamemode.h"

#include "Characters/Player/WizardPlayer.h"
#include "Kismet/GameplayStatics.h"

void ADungeonCrawlerGamemode::BeginPlay()
{
	Super::BeginPlay();

	const AWizardPlayer* Player = Cast<AWizardPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode: Failed to fetch player pawn"));
		return;
	}

	PlayerBag = Player->Bag;
	PlayerSpellBook = Player->SpellBook;
	UE_LOG(LogTemp, Warning, TEXT("GameMode: Set PlayerBag is %s"), PlayerBag.IsValid() ? TEXT("Valid") : TEXT("Invalid"));
	UE_LOG(LogTemp, Warning, TEXT("GameMode: Set PlayerSpellBook is %s"), PlayerSpellBook.IsValid() ? TEXT("Valid") : TEXT("Invalid"));
}
