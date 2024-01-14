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

	virtual void Interact() override;
	virtual void SetArmor(AClothes* NewClothes, EArmorTarget ArmorTarget) override;
public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USceneComponent* FP_RootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USpringArmComponent* ArmsSpringComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USceneComponent* ArmsOffsetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USkeletalMeshComponent* ArmsMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USpringArmComponent* CameraSpringComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USkeletalMeshComponent* CameraSkeletonComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	class UCameraComponent* CameraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	UPlayerInteractionRaycast* PlayerInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Components")
	USceneComponent* WeaponSocket;
	
protected:
	TWeakObjectPtr<AWeapon> CurrentWeapon;

private:
	float TargetCameraFieldOfView = 90.f;
};
