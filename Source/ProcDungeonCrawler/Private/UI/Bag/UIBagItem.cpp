// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Bag/UIBagItem.h"

#include "Components/TextBlock.h"

void UUIBagItem::SetItem(TSubclassOf<AActor> ItemClass, int32 Amount)
{
	ItemName->SetText(FText::FromString(ItemClass->GetName()));
	ItemAmount->SetText(FText::FromString(FString::FromInt(Amount)));
}
