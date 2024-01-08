// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Props/InteractionShapeComponent.h"

UInteractionShapeComponent::UInteractionShapeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInteractionShapeComponent::Interact(APawn* Pawn)
{
	
	return true;
}


void UInteractionShapeComponent::BeginPlay()
{
	Super::BeginPlay();
}
