
#include "UI/Wizard/RuneCastSlot.h"

#include "Spell/Rune.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URuneCastSlot::SetRuneData(URune* NewRuneCast)
{
	RuneCast = NewRuneCast;
	RuneIcon = RuneCast->RuneIcon;
	RuneName = FText::FromName(RuneCast->RuneName);
	
	RuneNameTextBlock->SetText(RuneName);
	
	FSlateBrush Brush = RuneIconImage->GetBrush();
	Brush.SetResourceObject(RuneIcon.LoadSynchronous());
	RuneIconImage->SetBrush(Brush);
}

void URuneCastSlot::ClearRuneData()
{
	RuneCast = nullptr;
	
	FSlateBrush Brush = RuneIconImage->GetBrush();
	Brush.SetResourceObject(nullptr);
	RuneIconImage->SetBrush(Brush);
	RuneName = FText::FromString("Empty");

	RuneNameTextBlock->SetText(RuneName);
}

void URuneCastSlot::SetSlotIndex(int Index)
{
	RuneIndex = Index;
	
	RuneIdTextBlock->SetText(FText::FromString(FString::FromInt(RuneIndex + 1)));
	RuneNameTextBlock->SetText(RuneName);
}

URune* URuneCastSlot::UseRune()
{
	if (IsEmpty()) return nullptr;

	// Broadcasts to trigger 
	if (OnTriggerSlotAnimation.IsBound()) {
		OnTriggerSlotAnimation.Broadcast(RuneIndex);
	}
	
	return RuneCast;
}

bool URuneCastSlot::IsEmpty()
{
	return RuneCast == nullptr;
}
