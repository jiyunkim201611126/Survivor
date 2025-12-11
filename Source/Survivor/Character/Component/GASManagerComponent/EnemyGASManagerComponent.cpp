// KJY

#include "EnemyGASManagerComponent.h"

#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/Character/SVEnemy.h"

void UEnemyGASManagerComponent::InitAbilityActorInfo()
{
	AbilitySystemComponent = GetPawn<ASVEnemy>()->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), GetOwner());
	Cast<USVAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	ApplyEffectToSelf(DefaultAttributes, 1.f);
}
