// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Wizard/WizardHUD.h"

#include "Blueprint/WidgetTree.h"
#include "UI/Wizard/RuneCastSlot.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "UI/Wizard/RuneCastsHistory.h"

void UWizardHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (RuneSlots != nullptr) {
		int ChildIdx = 0;
		for (UWidget* ChildWidget : RuneSlots->GetAllChildren()) {
			if (URuneCastSlot* RuneCastSlot = Cast<URuneCastSlot>(ChildWidget)) {
				RuneCastSlot->SetSlotIndex(ChildIdx);
				RuneCastSlots.Add(RuneCastSlot);
				ChildIdx++;
			}
		}
	}
}

void UWizardHUD::UseRuneOfIdx(int Idx)
{
	if (RuneSlots == nullptr || Idx < 0 || Idx > RuneCastSlots.Num()-1) return;

	URuneCastSlot* RuneCastSlot = RuneCastSlots[Idx];
	if (URuneCast* RuneCast = RuneCastSlot->UseRune())
	{
		CastedRuneHistory->AddRuneToCastHistory(RuneCast);
	}
}

bool UWizardHUD::IsSlotEmpty(int SlotIdx) const
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return false;
	
	URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx];
	return RuneCastSlot->IsEmpty();
}

void UWizardHUD::UnbindRuneToSlot(int SlotIdx)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;

	RuneCastSlots[SlotIdx]->ClearRuneData();
}

void UWizardHUD::BindRuneToSlot(int SlotIdx, URuneCast* RuneCast)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;
	
	if (URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx]) {
		RuneCastSlot->SetRuneData(RuneCast);
	}
}
