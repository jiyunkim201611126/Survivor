//KJY

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FSVGameplayTags
{
	static const FSVGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	//~ Begin Attributes
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_MaxHealth;
	
	FGameplayTag Attributes_Stat_Attack;
	FGameplayTag Attributes_Stat_Armor;

	FGameplayTag Attributes_Stat_Shield;
	FGameplayTag Attributes_Stat_MaxShield;
	FGameplayTag Attributes_Stat_AdditionalSpeed;
	//~ End of Attributes
	
	//~ Begin Damage Types
	FGameplayTag Damage_Normal;
	//~ End of Damage Types
	
	//~ Begin Abilities	
	FGameplayTag Abilities;
	//~ End of Ability
	
	//~ Begin Ability Types
	FGameplayTag Abilities_Types;
	//~ End Ability Types

	
	//~ Begin Enemy Abilities	
	FGameplayTag EnemyAbilities_Normal;
	FGameplayTag EnemyAbilities_Cooldown_Normal;
	//~ End of Enemy Abilities

	FGameplayTag CharacterState_Dead;

private:
	static FSVGameplayTags GameplayTags;
};
