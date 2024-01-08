// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Wizard/RuneCastSlotsContainer.h"

#include "Components/HorizontalBox.h"
#include "..\..\..\Public\UI\Wizard\RuneCastSlot.h"

void URuneCastSlotsContainer::PostLoad()
{
	Super::PostLoad();

	//Register slots.
	RegisterAllRunCastSlots();
}

void URuneCastSlotsContainer::AddNewRune(URune* RuneCast)
{
	if (RuneCastAvailableSlotIds.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find available slot for rune cast"));
		return;
	}
	const int SlotId = RuneCastAvailableSlotIds.Pop();
	// RuneCastSlots[SlotId]->SetRuneData(RuneCast);

	UE_LOG(LogTemp, Warning, TEXT("Added rune ui to slot %i for %s"), SlotId, *RuneCast->GetName());
}

void URuneCastSlotsContainer::RegisterAllRunCastSlots()
{
	if (!RuneSlotContainer1 || !RuneSlotContainer2 || !RuneSlotContainer3)
	{
		return;
	}

	int SlotIndex = RuneSlotContainer1->GetChildrenCount() + RuneSlotContainer2->GetChildrenCount() + RuneSlotContainer3->GetChildrenCount() - 1;
	int WidgetIndex = 0;
	for (UWidget* Widget : RuneSlotContainer1->GetAllChildren())
	{
		if (URuneCastSlot* RuneCastSlot = Cast<URuneCastSlot>(Widget))
		{
			WidgetIndex++;
			
			RuneCastSlots.Add(RuneCastSlot);
			RuneCastAvailableSlotIds.Add(SlotIndex);
			SlotIndex--;
		}
	}

	for (UWidget* Widget : RuneSlotContainer2->GetAllChildren())
	{
		if (URuneCastSlot* RuneCastSlot = Cast<URuneCastSlot>(Widget))
		{
			WidgetIndex++;
			
			RuneCastSlots.Add(RuneCastSlot);
			RuneCastAvailableSlotIds.Add(SlotIndex);
			SlotIndex--;
		}
	}

	for (UWidget* Widget : RuneSlotContainer3->GetAllChildren())
	{
		if (URuneCastSlot* RuneCastSlot = Cast<URuneCastSlot>(Widget))
		{
			WidgetIndex++;
			
			RuneCastSlots.Add(RuneCastSlot);
			RuneCastAvailableSlotIds.Add(SlotIndex);
			SlotIndex--;
		}
	}
}

void URuneCastSlotsContainer::ChangeVisibleSlot(int SlotIndex)
{
	if (!RuneSlotContainer1 || !RuneSlotContainer2 || !RuneSlotContainer3)
	{
		return;
	}

	const float SlotHeight = RuneSlotContainer1->GetDesiredSize().Y;
	UE_LOG(LogTemp, Display, TEXT("Slot height is: %f"), SlotHeight);

	VisibleSlot = SlotIndex;
}
