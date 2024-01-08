// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Human/Human.h"
#include "PlayerPawn.generated.h"

class UInventoryComponent;
class AWeapon;
class USpringArmComponent;
class UPlayerInteractionRaycast;
class UPhysicsHandleComponent;

UENUM()
enum class EHandsVisibility
{
	E_Hidden,
	E_Transition,
	E_Visible
};

class UInputMappingContext;
class UBagComponent;
class UWidgetInteractionComponent;
class UInputAction;
struct FInputActionValue;


UCLASS(Blueprintable, BlueprintType)
class APlayerPawn : public AHuman
{
	GENERATED_BODY()
	
public:
	APlayerPawn();
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
	
public:
	// Components
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraArmComponent;
	
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* ArmsMeshComponent;
	UPROPERTY(EditAnywhere)
	UPlayerInteractionRaycast* PlayerInteraction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetInteractionComponent* WidgetInteractionComponent;
	
protected:
	TWeakObjectPtr<AWeapon> CurrentWeapon;

private:
	float TargetCameraFieldOfView = 90.f;
};
