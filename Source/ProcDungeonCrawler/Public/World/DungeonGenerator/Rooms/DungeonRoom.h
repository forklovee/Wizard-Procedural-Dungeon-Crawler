
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
		StartPoint = 0.f;
		EndPoint = 0.f;
	}

	FWallRange(float NewStartPoint, float NewEndPoint)
	{
		if (NewStartPoint < NewEndPoint){
			StartPoint = NewStartPoint;
			EndPoint = NewEndPoint;
		}
		else
		{
			StartPoint = NewEndPoint;
			EndPoint = NewStartPoint;
		}
	}

	float GetRangeLength() const
	{
		return abs(EndPoint - StartPoint);
	}
	
	float StartPoint;
	float EndPoint;
};

USTRUCT()
struct FRoomWall
{
	GENERATED_BODY()

	FVector StartPoint;
	FVector EndPoint;

	FRoomWall()
	{
		StartPoint = FVector::ZeroVector;
		EndPoint = FVector::ZeroVector;
	}
	FRoomWall(FVector NewStartPoint, FVector NewEndPoint)
	{
		StartPoint = NewStartPoint;
		EndPoint = NewEndPoint;
	}

	FORCEINLINE bool operator == (const FRoomWall& OtherRoomWall) const
	{
		return this->StartPoint == OtherRoomWall.StartPoint && this->EndPoint == OtherRoomWall.EndPoint;
	}
	
	void GetWallFreeRanges(TMap<FVector, TArray<FWallRange>>& OccupationMap, TArray<FWallRange>& OutFreeRanges)
	{
		const FVector WallNormal = this->GetWallNormal();
		if (!OccupationMap.Contains(WallNormal))
		{
			return;
		}

		TArray<FWallRange>& WallOccupiedRanges = OccupationMap[WallNormal];
		TArray<FWallRange> WallFreeRanges = TArray<FWallRange>();
		
		float FreePointStart = 0.0;		
		for (const FWallRange& WallOccupiedRange: WallOccupiedRanges)
		{
			const float FreePointEnd = WallOccupiedRange.StartPoint;

			// Check if Free Point in Free Ranges
			if (WallFreeRanges.Num() > 0 && !WallFreeRanges.ContainsByPredicate([=](const FWallRange& CheckFreeWallRange){
				return FreePointStart >= CheckFreeWallRange.StartPoint && FreePointStart <= CheckFreeWallRange.EndPoint ||
					   FreePointEnd >= CheckFreeWallRange.StartPoint && FreePointEnd <= CheckFreeWallRange.EndPoint;
			}))
			{
				WallFreeRanges.Add( FWallRange(FreePointStart, FreePointEnd) );
			}
			FreePointStart = WallOccupiedRange.EndPoint;
		}
		OutFreeRanges = WallFreeRanges;
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
		return GetStartToEndDirection().RotateAngleAxis(90.f, FVector::UpVector);;
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

	FVector GetRoomCenter() const;
	
	TArray<FRoomWall> GetRoomWalls() const;
	TArray<FRoomWall> GetRoomWallsOfNormal(const FVector& WallNormal) const;

	bool IsOverlappingWithRoom(const FRoomWall& ThisRoomWall, const ADungeonRoom* OtherRoom, const FRoomWall& OtherRoomWall) const;
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
	FName TargetRoomPCGTag = FName("Basic");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Room")
	uint8 RoomHeight = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	USplineComponent* RoomBaseSpline;

private:
	FTimerHandle BuildTimerHandle;
};
