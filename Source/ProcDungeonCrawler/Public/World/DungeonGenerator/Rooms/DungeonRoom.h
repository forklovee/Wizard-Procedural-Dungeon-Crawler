
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DungeonRoom.generated.h"

class ADungeonRoom;
class USplineComponent;
class UPCGComponent;

USTRUCT()
struct FWallRange
{
	GENERATED_BODY()

	FWallRange()
	{
		StartLerpAlpha = 0.0;
		EndLerpAlpha = 200.f;
	}

	FWallRange(const float _StartLerpAlpha, const float _EndLerpAlpha)
	{
		StartLerpAlpha = _StartLerpAlpha;
		EndLerpAlpha = _EndLerpAlpha;
	}

	FORCEINLINE bool operator == (const FWallRange& OtherWallRange) const
	{
		return this->StartLerpAlpha == OtherWallRange.StartLerpAlpha
			&& this->EndLerpAlpha == OtherWallRange.EndLerpAlpha;
	}
	
	bool IsPointInRange(const float LerpPoint) const
	{
		return LerpPoint >= StartLerpAlpha && LerpPoint <= EndLerpAlpha;
	}
	
	float StartLerpAlpha = 0.f;
	float EndLerpAlpha = 200.f;
};

USTRUCT()
struct FRoomWall
{
	GENERATED_BODY()
	
	// Local Space
	FVector StartPoint;
	FVector EndPoint;
	
	TWeakObjectPtr<class ADungeonRoom> ConnectedRoom;
	FRoomWall* ConnectedWall = nullptr;
	FVector DoorPosition;

	FRoomWall()
	{
		StartPoint = FVector::ZeroVector;
		EndPoint = FVector::ZeroVector;
	}
	
	FRoomWall(const FVector NewStartPoint, const FVector NewEndPoint)
	{
		this->StartPoint = NewStartPoint;
		this->EndPoint = NewEndPoint;
	}

	FORCEINLINE bool operator == (const FRoomWall& OtherRoomWall) const
	{
		return this->StartPoint == OtherRoomWall.StartPoint && this->EndPoint == OtherRoomWall.EndPoint;
	}
	
	float GetWallLength() const
	{
		return FVector::Dist(StartPoint, EndPoint);
	}
	FVector GetStartToEndDirection() const
	{
		return (EndPoint - StartPoint).GetSafeNormal();
	}
	FVector GetWallNormal() const
	{
		return GetStartToEndDirection().RotateAngleAxis(-90.f, FVector::UpVector).GetSafeNormal();
	}
	FVector GetWallDirection() const
	{
		return (EndPoint - StartPoint).GetSafeNormal();
	}
	float GetWallDotToForward() const
	{
		return FVector::DotProduct(GetWallNormal(), FVector::ForwardVector);
	}
	FVector GetWallCenter() const
	{
		return (StartPoint + EndPoint) / 2.f;
	}
	TArray<FVector> GetPointDirectionsFromWallCenter() const
	{
		const FVector WallCenter = GetWallCenter();
		return {
			(StartPoint - WallCenter).GetSafeNormal(),
			(EndPoint - WallCenter).GetSafeNormal()
		};
	}
	FVector GetPointFromDistance(const float& Distance) const
	{
		return StartPoint + GetWallDirection() * Distance;
	}

	void SetConnectedRoom(TWeakObjectPtr<ADungeonRoom> NewConnectedRoom, FRoomWall* NewConnectedWall, const FVector NewDoorPosition)
	{
		this->ConnectedRoom = NewConnectedRoom;
		this->ConnectedWall = NewConnectedWall;
		this->DoorPosition = NewDoorPosition;
	}
};

UCLASS()
class PROCDUNGEONCRAWLER_API ADungeonRoom : public AActor
{
	GENERATED_BODY()

public:
	ADungeonRoom();

	void DrawDebugShapes();
	
	FVector GetRoomCenter() const;
	
	TArray<FRoomWall>& GetRoomWalls();
	TArray<FRoomWall*> GetValidRoomWalls();
	TArray<FRoomWall*> GetRoomWallsOfNormal(const FVector& WallNormal);

	TArray<FVector> GetRoomLocalPoints();

	bool IsPointInsideRoom(const FVector& Point);
	bool IsOverlappingWithRoom(ADungeonRoom* OtherRoom);
	
protected:
	virtual void BeginPlay() override;
	void GenerateRoomWalls();

	void RotatePointAroundCenter(FVector& Point, const FVector& Center, const float AngleDeg) const;
	void ScaleWallByPoint(FRoomWall& RoomWall, FVector Point, float SizeChange) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
	FName TargetRoomPCGTag = FName("Basic");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
	uint8 RoomHeight = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	USplineComponent* RoomBoundsSpline;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USplineComponent* RoomHeightSpline;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USplineComponent* RoomBuildSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UPCGComponent* RoomWallsPCGComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UPCGComponent* AssetPlacerPCGComponent;
	
private:
	FTimerHandle BuildTimerHandle;

	TArray<FRoomWall> RoomWalls;

	float GridTileSize = 200.f;
	float MeshTileSize = 600.f;
};
