// KJY

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SVAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&);

UCLASS()
class SURVIVOR_API USVAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities);
	void AddCharacterAbilitiesWithActive(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities);

private:
	// OnGameplayEffectAppliedDelegateToSelf에 붙이는 함수, 해당 델리게이트는 Server에서만 호출하기 때문에 RPC를 바인드해 클라이언트도 메시지 표시
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

public:
	FEffectAssetTags EffectAssetTags;
};
