// KJY

#include "GASManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"

UGASManagerComponent::UGASManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGASManagerComponent::SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
}

void UGASManagerComponent::SetAttributeSet(UAttributeSet* InAttributeSet)
{
	AttributeSet = InAttributeSet;
}

UAbilitySystemComponent* UGASManagerComponent::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void UGASManagerComponent::InitAbilityActorInfo()
{
}

void UGASManagerComponent::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const
{
	check(IsValid(AbilitySystemComponent));
	check(GameplayEffectClass);

	const FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}

void UGASManagerComponent::AddCharacterStartupAbilities() const
{
	if (!HasAuthority())
	{
		return;
	}

	// ASC 가져와서 부여 함수 호출
	USVAbilitySystemComponent* SVASC = CastChecked<USVAbilitySystemComponent>(AbilitySystemComponent);
	SVASC->AddCharacterAbilities(StartupAbilities);
}
