
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PropInteractionEnabler.generated.h"

UINTERFACE(MinimalAPI)
class UPropInteractionEnabler : public UInterface
{
	GENERATED_BODY()
};

class PROCDUNGEONCRAWLER_API IPropInteractionEnabler
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	FText GetPropNameText();

};
