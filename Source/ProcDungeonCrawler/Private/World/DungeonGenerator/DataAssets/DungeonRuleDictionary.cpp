
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"

bool UDungeonRuleDictionary::HasFloorRoomStructures(const int Floor) const
{
	return PerFloorRoomStructures.Num() > Floor;
}

TArray<FRuleCollection>& UDungeonRuleDictionary::GetFloorRoomStructures(const int Floor)
{
	return PerFloorRoomStructures[Floor];
}

FRuleCollection& UDungeonRuleDictionary::GetRandomFloorRoomStructure(const int Floor)
{
	TArray<FRuleCollection>& RoomCollectionsOfType = GetFloorRoomStructures(Floor);
	return RoomCollectionsOfType[FMath::RandRange(0, RoomCollectionsOfType.Num() - 1)];
}

bool UDungeonRuleDictionary::HasRoomExtensionOfType(const ERoomType RoomType) const
{
	return RuleExtensions.ContainsByPredicate([=](const FRuleExtension& RuleCollection)
		{ return RuleCollection.RoomType == RoomType; }
	);
}

FRuleExtension UDungeonRuleDictionary::GetRandomRoomExtensionOfType(const ERoomType RoomType) const
{
	TArray<FRuleExtension> RoomExtensions = RuleExtensions.FilterByPredicate([=](const FRuleExtension& RuleCollection)
		{ return RuleCollection.RoomType == RoomType; }
	);

	return RoomExtensions[FMath::RandRange(0, RoomExtensions.Num() - 1)];
}