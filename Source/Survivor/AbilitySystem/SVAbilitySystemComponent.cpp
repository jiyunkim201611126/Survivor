// KJY

#include "SVAbilitySystemComponent.h"

#include "Abilities/SVGameplayAbility.h"

void USVAbilitySystemComponent::AbilityActorInfoSet()
{
	// GE가 적용되면 호출되는 델리게이트입니다.
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientEffectApplied);
}

void USVAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : InAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void USVAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	if (!Cast<APawn>(GetAvatarActor())->IsLocallyControlled())
	{
		return;
	}
	
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
