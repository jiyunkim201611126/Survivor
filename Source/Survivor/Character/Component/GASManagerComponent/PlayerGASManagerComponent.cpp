// KJY

#include "PlayerGASManagerComponent.h"

#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/Manager/SVGameplayTags.h"
#include "Survivor/Player/SVPlayerController.h"
#include "Survivor/Player/SVPlayerState.h"
#include "Survivor/UI/HUD/SVHUD.h"

void UPlayerGASManagerComponent::InitAbilityActorInfo()
{
	ASVPlayerState* SVPlayerState = GetPlayerState<ASVPlayerState>();
	check(SVPlayerState);

	AbilitySystemComponent = SVPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(SVPlayerState, GetPawn<APawn>());
	Cast<USVAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = SVPlayerState->GetAttributeSet();

	ApplyEffectToSelf(DefaultAttributes, SVPlayerState->GetPlayerLevel());

	// Ability는 쿨다운이 끝나면 자동으로 재발동합니다.
	// AbilitySystemComponent에 특정 태그가 부여/제거될 때 호출되는 델리게이트를 이용합니다.
	const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();
	for (const auto& CooldownToAbility : GameplayTags.CooldownTagToAbilityTag)
	{
		// Cooldown 태그의 변동 사항에 대해 콜백을 걸어놓습니다.
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownToAbility.Key, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCooldownTagChanged);
	}

	if (ASVPlayerController* SVPlayerController = GetController<ASVPlayerController>())
	{
		if (ASVHUD* SVHUD = Cast<ASVHUD>(SVPlayerController->GetHUD()))
		{
			SVHUD->InitHUD(SVPlayerController, SVPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void UPlayerGASManagerComponent::AddCharacterStartupAbilities() const
{
	if (!HasAuthority())
	{
		return;
	}

	// ASC 가져와서 부여 함수 호출
	USVAbilitySystemComponent* SVASC = CastChecked<USVAbilitySystemComponent>(AbilitySystemComponent);
	SVASC->AddCharacterAbilitiesWithActive(StartupAbilities);
}

void UPlayerGASManagerComponent::OnCooldownTagChanged(const FGameplayTag CooldownTag, const int32 NewCount) const
{
	if (NewCount == 0)
	{
		// CooldownTag를 받으면 해당하는 AbilityTag를 가져와 Activate합니다.
		const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();
		const FGameplayTag* AbilityTag = GameplayTags.CooldownTagToAbilityTag.Find(CooldownTag);
		AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTag->GetSingleTagContainer());
	}
}
