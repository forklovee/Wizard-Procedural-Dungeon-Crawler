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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spell")
	void ChangeCollisionProfileToSpellInteractive();
	virtual void ChangeCollisionProfileToSpellInteractive_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spell")
	void AddSpellHandleTag();
	virtual void AddSpellHandleTag_Implementation() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell", meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* PropStaticMesh;
};
