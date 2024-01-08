#include "Components/Props/SpellInteractionShapeComponent.h"

#include "Spell/Spell.h"

USpellInteractionShapeComponent::USpellInteractionShapeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpellInteractionShapeComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &USpellInteractionShapeComponent::OnOverlapBegin);
}

void USpellInteractionShapeComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
													 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
													 bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor->IsA(ASpell::StaticClass())) return;
}



