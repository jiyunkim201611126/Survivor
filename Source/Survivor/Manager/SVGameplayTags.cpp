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

	GameplayTags.DamageTypeTags.Emplace(GameplayTags.Damage_Normal);
	
	
	GameplayTags.Abilities_FloatingActor_Axe = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.FloatingActor.Axe"), FString(""));
	
	GameplayTags.Abilities_Types = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Types"), FString(""));


	GameplayTags.EnemyAbilities_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("EnemyAbilities.Normal"), FString(""));
	GameplayTags.EnemyAbilities_Cooldown_Normal = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("EnemyAbilities.Cooldown.Normal"), FString(""));
		
	GameplayTags.EnemyCooldownTagToAbilityTag.Emplace(GameplayTags.EnemyAbilities_Cooldown_Normal, GameplayTags.EnemyAbilities_Normal);
	
	GameplayTags.CharacterState_Dead = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CharacterState.Dead"), FString(""));
}
