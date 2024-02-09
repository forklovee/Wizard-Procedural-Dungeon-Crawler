// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ContextMenuActionButton.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UContextMenuActionButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ActionText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ActionTextBlock;
};
