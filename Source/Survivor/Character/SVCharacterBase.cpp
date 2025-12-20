// KJY

#include "SVCharacterBase.h"

#include "Component/GASManagerComponent/GASManagerComponent.h"

ASVCharacterBase::ASVCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASVCharacterBase::ApplyKnockback(const FVector& KnockbackForce)
{
	LaunchCharacter(KnockbackForce, true, true);
}

void ASVCharacterBase::InitAbilityActorInfo() const
{
	GASManagerComponent->SetAbilitySystemComponent(AbilitySystemComponent);
	GASManagerComponent->SetAttributeSet(AttributeSet);
	GASManagerComponent->InitAbilityActorInfo();
}

UAbilitySystemComponent* ASVCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
