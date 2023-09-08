
#include "..\..\..\Public\UI\Wizard\RuneCastSlot.h"

#include "WizardCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Spell/RuneCast.h"

void URuneCastSlot::SetRuneData(int _RuneIndex, URuneCast* _RuneCast)
{
	RuneCast = _RuneCast;
	
	RuneIndex = _RuneIndex;

	RuneIcon = RuneCast->RuneIcon;
	RuneName = RuneCast->RuneName;

	if (RuneIconImage)
	{
		FSlateBrush Brush =  RuneIconImage->GetBrush();
		Brush.SetResourceObject(RuneIcon.Get());

		RuneIconImage->SetBrush(Brush);
	}

	SetRuneCastSlotIndex(RuneIndex);
	
	RuneCast->OnRuneCasted.AddDynamic(this, &URuneCastSlot::TriggerUseAnimation);
}

void URuneCastSlot::SetRuneCastSlotIndex(int Index)
{
	RuneIndex = Index;
	if (RuneIdTextBlock)
	{
		RuneIdTextBlock->SetText(FText::FromString(FString::FromInt(RuneIndex + 1)));
	}
	
	if (RuneNameTextBlock)
	{
		RuneNameTextBlock->SetText(FText::FromName(RuneName));
	}
}

void URuneCastSlot::TriggerUseAnimation(AWizardCharacter* WizardCharacter, URuneCast* _RuneCast)
{
	if (!IsValid(WizardCharacter) || !IsValid(_RuneCast)) return;
}

bool URuneCastSlot::IsEmpty()
{
	return !RuneCast.IsNull();
}
