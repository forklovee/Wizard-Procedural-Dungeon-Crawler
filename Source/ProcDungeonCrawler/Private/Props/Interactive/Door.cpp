
#include "Props/Interactive/Door.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	
	DoorConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("DoorRoot"));
	DoorConstraint->SetupAttachment(RootComponent);
	DoorConstraint->SetAngularSwing1Limit(ACM_Limited, 150.f);
	
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorMesh"));
	DoorMesh->SetupAttachment(DoorConstraint);

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("FrameMesh"));
	FrameMesh->SetupAttachment(RootComponent);

	DoorOpenFinalYaw = 150.f;
}

FText ADoor::GetPropNameText_Implementation()
{
	return FText::FromString( GetClass()->GetName() );
}

bool ADoor::Interact_Implementation(APawn* Pawn)
{
	IPropInteractionInterface::Interact_Implementation(Pawn);

	const bool bIsDoorOpened = IsDoorOpened();
	if (bIsDoorOpened)
	{
		CloseDoor();		
	}
	else
	{
		OpenDoor(Pawn);
	}
	
	return bIsDoorOpened;
}

FText ADoor::GetInteractionNameText_Implementation()
{
	IPropInteractionInterface::GetInteractionNameText_Implementation();

	FText InteractionNameText = FText::FromString( "Open" );
	if (!IsDoorOpened())
	{
		InteractionNameText = FText::FromString( "Close" );	
	}
	return InteractionNameText;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ADoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float LockMargin = 1.f;
	const float CurrentYaw = DoorMesh->GetRelativeRotation().Yaw;
	const float CurrentYawAbs = FMath::Abs(CurrentYaw);
	const float TargetYawAbs = FMath::Abs(OpenDirection * DoorOpenFinalYaw);

	if (
		(TargetYawAbs != 0.f && CurrentYawAbs > DoorOpenFinalYaw - LockMargin) ||
		(TargetYawAbs == 0.f &&
			(LastOpenDirection == 1 && CurrentYaw < 0 || LastOpenDirection == -1 && CurrentYaw > 0)
			)
		)
	{
		DoorMesh->SetSimulatePhysics(false);
		SetActorTickEnabled(false);
		UE_LOG(LogTemp, Display, TEXT("LOCK!"));
	}
}

bool ADoor::IsDoorOpened() const
{
	return OpenDirection != 0;
}

void ADoor::OpenDoor(const APawn* Pawn)
{
	FVector Direction = FrameMesh->GetComponentLocation() - Pawn->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();
	const float Dot = FVector::DotProduct(Direction, GetActorRightVector());

	LastOpenDirection = OpenDirection;
	OpenDirection = Dot > 0 ? 1 : -1;
	const float Yaw = OpenDirection * DoorOpenFinalYaw;
	UE_LOG(LogTemp, Display, TEXT("OpenDoor: %f"), Yaw);
	DoorConstraint->SetAngularOrientationTarget(FRotator(0.f, Yaw, 0.f));
	
	DoorMesh->SetSimulatePhysics(true);
	SetActorTickEnabled(true);
}

void ADoor::CloseDoor()
{
	LastOpenDirection = OpenDirection;
	OpenDirection = 0;
	DoorConstraint->SetAngularOrientationTarget(FRotator(0.f, 0.f, 0.f));
	
	DoorMesh->SetSimulatePhysics(true);
	SetActorTickEnabled(true);
}

