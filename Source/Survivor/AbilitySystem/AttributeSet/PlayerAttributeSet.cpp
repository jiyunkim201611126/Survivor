// KJY

#include "PlayerAttributeSet.h"

#include "Survivor/Manager/SVGameplayTags.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();

	TagsToAttributes.Reserve(7);
	
	TagsToAttributes.Emplace(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Attack, GetAttackAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Armor, GetArmorAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_KnockbackResistance, GetKnockbackResistanceAttribute);
	
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Shield, GetShieldAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_MaxShield, GetMaxShieldAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_AdditionalSpeed, GetAdditionalSpeedAttribute);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UPlayerAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Shield, OldShield);
}

void UPlayerAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxShield, OldMaxShield);
}

void UPlayerAttributeSet::OnRep_AdditionalSpeed(const FGameplayAttributeData& OldAdditionalSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, AdditionalSpeed, OldAdditionalSpeed);
}
