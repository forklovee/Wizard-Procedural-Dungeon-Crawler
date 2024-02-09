// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ContextMenuActionButton.h"

#include "Components/TextBlock.h"

void UContextMenuActionButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(ActionTextBlock))
	{
		ActionTextBlock->SetText(ActionText);
	}
}
