// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Tabs/InventoryTabButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UInventoryTabButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (TabTextBlock)
	{
		TabTextBlock->SetText(TabText);
	}
}

void UInventoryTabButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button->OnPressed.AddDynamic(this, &UInventoryTabButton::OnTabButtonPressed);
	Button->OnHovered.AddDynamic(this, &UInventoryTabButton::OnTabButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &UInventoryTabButton::OnTabButtonUnhovered);
}

void UInventoryTabButton::TogglePressed()
{
	SetPressed(!bIsButtonPressed);
}

void UInventoryTabButton::SetPressed(const bool bNewIsPressed)
{
	bIsButtonPressed = bNewIsPressed;
	
	OnTabButtonPressed();
}




