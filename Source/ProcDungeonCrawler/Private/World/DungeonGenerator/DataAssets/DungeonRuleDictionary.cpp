
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"

int UDungeonRuleDictionary::GetFloorAmount() const
{
	return PerFloorRoomStructures.Num();
}

bool UDungeonRuleDictionary::HasFloorRoomStructures(const int Floor) const
{
	return GetFloorAmount() > Floor;
}

FRuleCollection& UDungeonRuleDictionary::GetFloorRoomStructure(const int Floor)
{
	return PerFloorRoomStructures[Floor];
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