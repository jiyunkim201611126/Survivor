//KJY

#include "SVGameplayTags.h"
#include "GameplayTagsManager.h"

FSVGameplayTags FSVGameplayTags::GameplayTags;

void FSVGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.Health"), FString(""));
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxHealth"), FString(""));
	
	GameplayTags.Attributes_Stat_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Stat.Attack"), FString(""));
	GameplayTags.Attributes_Stat_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Stat.Armor"), FString(""));
	
	GameplayTags.Attributes_Stat_Shield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Stat.Shield"), FString(""));
	GameplayTags.Attributes_Stat_MaxShield = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Stat.MaxShield"), FString(""));
	GameplayTags.Attributes_Stat_AdditionalSpeed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Stat.AdditionalSpeed"), FString(""));

	
	GameplayTags.Damage_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Normal"), FString(""));

	
	GameplayTags.Abilities_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.None"), FString(""));

	
	GameplayTags.Abilities_Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Cooldown"), FString(""));

	
	GameplayTags.Abilities_Types = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Types"), FString(""));
}
