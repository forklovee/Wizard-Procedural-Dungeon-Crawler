
#include "World/DungeonGenerator/DataAssets/DungeonRuleDictionary.h"

bool UDungeonRuleDictionary::HasRoomCollectionOfType(const ERoomType RoomType) const
{
	return RuleCollections.ContainsByPredicate([=](const FRuleCollection& RuleCollection)
		{ return RuleCollection.RoomType == RoomType; }
	);
}

FRuleCollection UDungeonRuleDictionary::GetRandomRoomCollectionByType(const ERoomType RoomType) const
{
	const TArray<FRuleCollection> RoomCollectionsOfType = GetRoomCollectionsByType(RoomType);
	if (RoomCollectionsOfType.Num() == 0) return FRuleCollection();

	return RoomCollectionsOfType[FMath::RandRange(0, RoomCollectionsOfType.Num() - 1)];
}

TArray<FRuleCollection> UDungeonRuleDictionary::GetRoomCollectionsByType(const ERoomType RoomType) const
{
	return RuleCollections.FilterByPredicate([=] (const FRuleCollection& RuleCollection)
		{ return RuleCollection.RoomType == RoomType; }
		);
}
