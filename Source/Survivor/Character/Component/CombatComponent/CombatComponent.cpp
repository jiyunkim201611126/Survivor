// KJY

#include "CombatComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"

UCombatComponent::UCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCombatComponent::SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
}

void UCombatComponent::SetAttributeSet(UAttributeSet* InAttributeSet)
{
	AttributeSet = InAttributeSet;
}

UAbilitySystemComponent* UCombatComponent::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void UCombatComponent::InitAbilityActorInfo()
{
}

void UCombatComponent::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const
{
	check(IsValid(AbilitySystemComponent));
	check(GameplayEffectClass);

	const FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}

void UCombatComponent::AddCharacterStartupAbilities() const
{
	if (!HasAuthority())
	{
		return;
	}

	// ASC 가져와서 부여 함수 호출
	USVAbilitySystemComponent* SVASC = CastChecked<USVAbilitySystemComponent>(AbilitySystemComponent);
	SVASC->AddCharacterAbilities(StartupAbilities);
}
