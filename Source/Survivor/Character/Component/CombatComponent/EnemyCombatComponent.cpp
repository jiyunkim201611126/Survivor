// KJY

#include "EnemyCombatComponent.h"

#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/Character/SVEnemy.h"

void UEnemyCombatComponent::InitAbilityActorInfo()
{
	AbilitySystemComponent = GetPawn<ASVEnemy>()->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), GetOwner());
	Cast<USVAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	ApplyEffectToSelf(DefaultAttributes, 1.f);
}
