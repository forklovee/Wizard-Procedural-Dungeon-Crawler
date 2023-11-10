// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell/SpellCast.h"
#include "RangedSpell.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API ARangedSpell : public ASpellCast
{
	GENERATED_BODY()

public:
	ARangedSpell();

	virtual void BeginPlay() override;
	virtual void CastSpell(AWizardCharacter* WizardCharacter) override;

	UFUNCTION()
	virtual void OnStaticMeshHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnSpellHitboxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSpellHitboxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditAnywhere, Category="Spell Hitbox", meta=(AllowPrivateAccess="true"))
	USphereComponent* SpellHitBoxComponent;

	UPROPERTY(EditAnywhere, Category="Spell Hitbox", meta=(AllowPrivateAccess="true"))
	USphereComponent* StaticMeshHitBoxComponent;
};
