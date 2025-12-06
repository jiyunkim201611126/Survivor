// KJY

#include "OverlayWidgetController.h"

#include "Survivor/Player/SVPlayerState.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/PlayerAttributeSet.h"

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

	// 선언된 Attribute들에게 변동사항이 있는 경우 Widget Controller가 알 수 있도록 각 Attribute에게 함수를 바인드합니다.
	SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetSVPlayerAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	
	SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetSVPlayerAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});

	SVAbilitySystemComponent->EffectAssetTags.AddUObject(this, &ThisClass::OnEffectHasMessageTagApplied);
}

void UOverlayWidgetController::BroadcastInitialValue()
{
	OnHealthChanged.Broadcast(SVAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(SVAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::OnXPChanged(int32 InXP)
{
	// 임시
	const float XPBarPercent = 0.2f;
	OnXPBarPercentChangedDelegate.Broadcast(XPBarPercent);
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
