// KJY

#include "OverlayWidgetController.h"

#include "Survivor/Player/SVPlayerState.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/PlayerAttributeSet.h"
#include "Survivor/Player/SVPlayerController.h"

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	if (SVPlayerState)
	{
		SVPlayerState->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);
		SVPlayerState->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel, bool bLevelUp)
			{
				OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
			});
	}

	// Attribute들에게 변동사항이 있는 경우 Widget Controller가 알 수 있도록 각 AttributeSet에게 함수를 바인드합니다.
	SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetSVPlayerAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	
	SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetSVPlayerAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
	
	SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetSVPlayerAS()->GetShieldAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnShieldChanged.Broadcast(Data.NewValue);
		});
	
	SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetSVPlayerAS()->GetMaxShieldAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxShieldChanged.Broadcast(Data.NewValue);
		});

	SVAbilitySystemComponent->EffectAssetTags.AddUObject(this, &ThisClass::OnEffectHasMessageTagApplied);
}

void UOverlayWidgetController::BroadcastInitialValue()
{
	OnHealthChanged.Broadcast(SVAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(SVAttributeSet->GetMaxHealth());
	OnShieldChanged.Broadcast(SVAttributeSet->GetShield());
	OnMaxShieldChanged.Broadcast(SVAttributeSet->GetMaxShield());
	OnXPChanged(SVPlayerState->GetXP());
}

void UOverlayWidgetController::OnPlayButtonClicked() const
{
	SVPlayerController->PauseOrRestartGame();
}

void UOverlayWidgetController::OnXPChanged(int32 InXP)
{
	// 임시
	int32 CurrentXP = 10.f;
	int32 MaxXP = 100.f;
	OnXPChangedDelegate.Broadcast(CurrentXP, MaxXP);
}

void UOverlayWidgetController::OnEffectHasMessageTagApplied(const FGameplayTagContainer& GameplayTags) const
{
	const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
	for (const FGameplayTag& Tag : GameplayTags)
	{
		if (Tag.MatchesTag(MessageTag))
		{
			const FUIWidgetRow* Row = MessageWidgetDataTable->FindRow<FUIWidgetRow>(Tag.GetTagName(), TEXT(""));
			MessageWidgetRowDelegate.Broadcast(*Row);
		}
	}
}
