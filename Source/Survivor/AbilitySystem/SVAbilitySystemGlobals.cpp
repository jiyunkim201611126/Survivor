// KJY

#include "SVAbilitySystemGlobals.h"

#include "Survivor/SVAbilityTypes.h"

FGameplayEffectContext* USVAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FSVGameplayEffectContext();
}
