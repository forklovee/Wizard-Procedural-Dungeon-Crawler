// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Wizard/RuneIcon.h"

#include "Components/Image.h"

void URuneIcon::SetRuneIcon(UTexture2D* Icon)
{
	if (RuneIcon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("RuneIcon is nullptr"));
		return;
	}
	RuneIcon->SetBrushFromTexture(Icon);
}
