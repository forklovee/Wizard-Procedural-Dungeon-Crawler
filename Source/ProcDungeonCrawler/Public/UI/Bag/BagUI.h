// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BagUI.generated.h"

class ABagActor;
class APickupItem;
class UScrollBox;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class PROCDUNGEONCRAWLER_API UBagUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupBagUI(ABagActor* BagActor, TArray<TSubclassOf<APickupItem>>& ItemClasses);

	TArray<APickupItem*> GetAllSpawnedActors() const;

	UFUNCTION()
	void ChangePage(int Direction);
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BagItemWidgetClass;
	
public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UScrollBox* ScrollBox;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UPanelWidget* BagItemsPanel;

private:
	int CurrentPage = 0;
};
