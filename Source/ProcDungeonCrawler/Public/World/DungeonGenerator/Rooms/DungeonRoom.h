
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DungeonRoom.generated.h"

class USplineComponent;
class UPCGComponent;

USTRUCT()
struct FWallRange
{
	GENERATED_BODY()

	FWallRange()
	{
		StartLerpPoint = 0.0;
		RangeEndLerpPoint = 1.0;
	}

	FWallRange(const float _StartLerpPoint, const float _RangeEndLerpPoint)
	{
		StartLerpPoint = _StartLerpPoint;
		RangeEndLerpPoint = FMath::Clamp(_RangeEndLerpPoint, 0.0, 1.0);
	}

	FORCEINLINE bool operator == (const FWallRange& OtherWallRange) const
	{
		return this->StartLerpPoint == OtherWallRange.StartLerpPoint
			&& this->RangeEndLerpPoint == OtherWallRange.RangeEndLerpPoint;
	}
	
	bool IsLerpPointInRange(const float LerpPoint) const
	{
		return LerpPoint >= StartLerpPoint && LerpPoint <= RangeEndLerpPoint;
	}

	float GetRangeLength() const
	{
		return RangeEndLerpPoint - StartLerpPoint;
	}
	
	float StartLerpPoint = 0.0;
	float RangeEndLerpPoint = 1.0;
};

USTRUCT()
struct FRoomWall
{
	GENERATED_BODY()

	FVector StartPoint;
	FVector EndPoint;

	TArray<FWallRange> FreeRanges;

	FRoomWall()
	{
		StartPoint = FVector::ZeroVector;
		EndPoint = FVector::ZeroVector;
		FreeRanges = {};
	}
	
	FRoomWall(FVector NewStartPoint, FVector NewEndPoint)
	{
		StartPoint = NewStartPoint;
		EndPoint = NewEndPoint;
		FreeRanges.Add(FWallRange());
	}

	void UseWallRange(FWallRange& WallRange, const float RangeStart, const float RangeDistance)
	{
		if (RangeDistance < WallRange.GetRangeLength()) //Split!
		{
			if (RangeStart == WallRange.StartLerpPoint)
			{
				WallRange.StartLerpPoint += RangeDistance;
			}
			else if (RangeStart + RangeDistance == WallRange.RangeEndLerpPoint)
			{
				WallRange.RangeEndLerpPoint = RangeStart;
			}
			else
			{
				FreeRanges.Add(FWallRange(WallRange.StartLerpPoint, RangeStart));
				FreeRanges.Add(FWallRange(RangeStart + RangeDistance, WallRange.RangeEndLerpPoint));
				FreeRanges.Remove(WallRange);
			}
		}
		else //Remove
		{
			FreeRanges.Remove(WallRange);
		}
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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
	FName TargetRoomPCGTag = FName("Basic");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
	uint8 RoomHeight = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USplineComponent* RoomBaseSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UPCGComponent* RoomWallsPCGComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UPCGComponent* AssetPlacerPCGComponent;
	
private:
	FTimerHandle BuildTimerHandle;

	TArray<FRoomWall> RoomWalls;
};
