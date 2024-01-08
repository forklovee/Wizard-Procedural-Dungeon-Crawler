// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Clothes/Clothes.h"


AClothes::AClothes(): AItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AClothes::BeginPlay()
{
	Super::BeginPlay();
	
}

