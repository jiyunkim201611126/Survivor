// KJY

#include "EnemyGASManagerComponent.h"

#include "Components/CapsuleComponent.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/Character/SVEnemy.h"
#include "Survivor/Manager/SVGameplayTags.h"

void UEnemyGASManagerComponent::InitAbilityActorInfo()
{
	const ASVEnemy* OwnerEnemy = GetPawn<ASVEnemy>();
	AbilitySystemComponent = OwnerEnemy->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(GetOwner(), GetOwner());
	Cast<USVAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	ApplyEffectToSelf(DefaultAttributes, 1.f);
	
	OwnerEnemy->GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	// Ability는 쿨다운이 끝나면 자동으로 재발동합니다.
	// AbilitySystemComponent에 특정 태그가 부여/제거될 때 호출되는 델리게이트를 이용합니다.
	const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();
	for (const auto& CooldownToAbility : GameplayTags.EnemyCooldownTagToAbilityTag)
	{
		// Cooldown 태그의 변동 사항에 대해 콜백을 걸어놓습니다.
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownToAbility.Key, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCooldownTagChanged);
	}
}

void UEnemyGASManagerComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 이 이벤트로 들어왔다면 OtherActor는 반드시 PlayerCharacter여야 합니다.
	const ASVEnemy* OwnerEnemy = GetPawn<ASVEnemy>();
	if (OwnerEnemy && OtherActor && OtherActor->Implements<UCombatInterface>() && AbilitySystemComponent->AbilityActorInfo.IsValid())
	{
		FScopedAbilityListLock AbilityListLock(*AbilitySystemComponent);
		const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();

		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.Ability->GetAssetTags().HasTagExact(GameplayTags.EnemyAbilities_Normal))
			{
				FGameplayEventData EventData;
				EventData.Target = OtherActor;

				AbilitySystemComponent->TriggerAbilityFromGameplayEvent(Spec.Handle, AbilitySystemComponent->AbilityActorInfo.Get(), FGameplayTag(), &EventData, *AbilitySystemComponent);

				// Ability가 발동되었으므로, Cooldown이 종료될 때까지 잠시 Overlap이 발생하지 않도록 설정합니다.
				OwnerEnemy->GetCapsuleComponent()->SetGenerateOverlapEvents(false);
				return;
			}
		}
	}
}

void UEnemyGASManagerComponent::OnCooldownTagChanged(const FGameplayTag CooldownTag, const int32 NewCount) const
{
	if (NewCount == 0)
	{
		// Cooldown이 종료되었으므로 다시 Overlap 이벤트가 발생할 수 있도록 설정합니다.
		if (const ASVEnemy* OwnerEnemy = GetPawn<ASVEnemy>())
		{
			OwnerEnemy->GetCapsuleComponent()->SetGenerateOverlapEvents(true);
		}
	}
}
