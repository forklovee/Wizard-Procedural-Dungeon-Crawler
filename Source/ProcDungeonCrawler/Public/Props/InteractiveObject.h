// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EInteractionFailReason: uint8
{
	None,
	NoItem,
	NoStats,
};

class APickupItem;
class AHuman;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionSuccess, AHuman*, HumanCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionFail, AHuman*, HumanCharacter, EInteractionFailReason, FailReason);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectDestroyed);

UCLASS()
class PROCDUNGEONCRAWLER_API AInteractiveObject : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractionSuccess OnInteractionSuccess;
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFail OnInteractionFail;
	
	UPROPERTY(BlueprintAssignable)
	FOnObjectDestroyed OnObjectDestroyed;
	
	AInteractiveObject();
	
	UFUNCTION()
	virtual void Interact(AHuman* HumanCharacter);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnDestroyed();
	virtual void OnDestroyed_Implementation();

	virtual bool CanInteract(AHuman* HumanCharacter, EInteractionFailReason& OutFailReason) const;

	UFUNCTION(BlueprintNativeEvent)
	void OnInteractionSuccessEvent(AHuman* HumanCharacter);
	virtual void OnInteractionSuccessEvent_Implementation(AHuman* HumanCharacter);

	UFUNCTION(BlueprintNativeEvent)
	void OnInteractionFailEvent(AHuman* HumanCharacter, EInteractionFailReason FailReason);
	virtual void OnInteractionFailEvent_Implementation(AHuman* HumanCharacter, EInteractionFailReason FailReason);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	bool bCanBeDestroyed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	float Durability = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactivity")
	TMap<TSubclassOf<APickupItem>, int32> InteractionRequiredItems;
};
