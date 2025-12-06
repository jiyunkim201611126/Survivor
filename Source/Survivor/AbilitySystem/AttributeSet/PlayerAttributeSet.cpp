// KJY

#include "PlayerAttributeSet.h"

#include "Survivor/Manager/SVGameplayTags.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();
	
	TagsToAttributes.Emplace(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Attack, GetAttackAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Armor, GetArmorAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_AdditionalSpeed, GetAdditionalSpeedAttribute);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}
