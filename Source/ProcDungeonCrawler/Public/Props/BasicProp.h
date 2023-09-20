// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/SpellHandleInterface.h"
#include "BasicProp.generated.h"

UCLASS()
class PROCDUNGEONCRAWLER_API ABasicProp : public AActor, public ISpellHandleInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicProp();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spell")
	void HandleSpellCast(class AWizardCharacter* WizardCharacter, class ASpellCast* SpellCast);
	virtual void HandleSpellCast_Implementation(class AWizardCharacter* WizardCharacter, class ASpellCast* SpellCast) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
