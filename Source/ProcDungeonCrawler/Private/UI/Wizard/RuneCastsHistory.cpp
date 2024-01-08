// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Wizard/RuneCastsHistory.h"

#include "Blueprint/WidgetTree.h"
#include "..\..\..\Public\Spell\Rune.h"
#include "UI/Wizard/RuneIcon.h"

void URuneCastsHistory::UpdateRuneCastHistory(const TArray<URune*>& CastedRunes)
{
	
}

void URuneCastsHistory::AddRuneToCastHistory(const URune* RuneCast) const
{
	if(URuneIcon* RuneIcon = WidgetTree->ConstructWidget<URuneIcon>(URuneIcon::StaticClass(), TEXT("aaa")))
	{
		RuneIcon->SetRuneIcon(RuneCast->RuneIcon.LoadSynchronous());
		RuneCastsHistoryPanel->AddChild(RuneIcon);
	}
}

void URuneCastsHistory::ClearCastHistory()
{
	for (UWidget* ChildWidget : RuneCastsHistoryPanel->GetAllChildren()) {
		ChildWidget->RemoveFromParent();
	}
}