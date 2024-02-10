// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTabButton.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class PROCDUNGEONCRAWLER_API UInventoryTabButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPressed(const bool bNewIsPressed);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTabButtonHovered();
	UFUNCTION(BlueprintImplementableEvent)
	void OnTabButtonUnhovered();
	UFUNCTION(BlueprintImplementableEvent)
	void OnTabButtonPressed();

	UFUNCTION(BlueprintCallable)
	bool IsButtonPressed() const { return bIsButtonPressed; }
private:
	
	void TogglePressed();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TabText = FText::FromString("Tab");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TabTextBlock;

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bIsButtonPressed = false;
};
