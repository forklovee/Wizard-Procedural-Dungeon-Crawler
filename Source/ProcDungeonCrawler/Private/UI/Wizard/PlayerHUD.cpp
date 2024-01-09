// Fill out your copyright notice in the Description page of Project Settings.


#include "..\..\..\Public\UI\Wizard\PlayerHUD.h"

#include "Blueprint/WidgetTree.h"
#include "UI/Wizard/RuneCastSlot.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "UI/Wizard/RuneCastsHistory.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// if (RuneSlots != nullptr) {
	// 	int ChildIdx = 0;
	// 	for (UWidget* ChildWidget : RuneSlots->GetAllChildren()) {
	// 		if (URuneCastSlot* RuneCastSlot = Cast<URuneCastSlot>(ChildWidget)) {
	// 			RuneCastSlot->SetSlotIndex(ChildIdx);
	// 			RuneCastSlots.Add(RuneCastSlot);
	// 			ChildIdx++;
	// 		}
	// 	}
	// }
}

void UPlayerHUD::OnRuneCasted(int RuneIdx, URune* RuneCast, TArray<URune*>& CastedRunes)
{
	// if (RuneSlots == nullptr || RuneIdx < 0 || RuneIdx > RuneCastSlots.Num()-1) return;
	//
	// URuneCastSlot* RuneCastSlot = RuneCastSlots[RuneIdx];
	// RuneCastSlot->UseRune();
	//
	// CastedRuneHistory->UpdateRuneCastHistory(CastedRunes);
}

bool UPlayerHUD::IsSlotEmpty(int SlotIdx) const
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return false;
	
	URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx];
	return RuneCastSlot->IsEmpty();
}

void UPlayerHUD::UnbindRuneToSlot(int SlotIdx)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;

	RuneCastSlots[SlotIdx]->ClearRuneData();
}

void UPlayerHUD::OnPlayerHeal(float Health, float HealHealh)
{
	if (OnPlayerHeal_UpdateHealth.IsBound())
	{
		OnPlayerHeal_UpdateHealth.Broadcast(Health, HealHealh);
	}
}

void UPlayerHUD::OnPlayerHurt(AActor* DamageCauser, float Damage, const UDamageType* DamageType)
{
	if (OnPlayerHurt_UpdateHealth.IsBound())
	{
		OnPlayerHurt_UpdateHealth.Broadcast(DamageCauser, Damage, DamageType);
	}
}

void UPlayerHUD::OnPlayerManaUsage(float Mana, float ManaUsage)
{
	if (OnPlayerManaUsage_ManaUpdate.IsBound())
	{
		OnPlayerManaUsage_ManaUpdate.Broadcast(Mana, ManaUsage);
	}
}

void UPlayerHUD::BindRuneToSlot(int SlotIdx, URune* RuneCast)
{
	if (SlotIdx < 0 || SlotIdx > RuneCastSlots.Num()-1) return;
	
	if (URuneCastSlot* RuneCastSlot = RuneCastSlots[SlotIdx]) {
		RuneCastSlot->SetRuneData(RuneCast);
	}
}
