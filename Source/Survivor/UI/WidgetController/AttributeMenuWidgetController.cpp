// KJY

#include "AttributeMenuWidgetController.h"

#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/PlayerAttributeSet.h"
#include "Survivor/AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// 하나의 Attribute만 변경되어도 여러 Attribute에게 영향을 줄 수 있으므로 모두 새로고침해야 합니다.
	check(AttributeInfo);
	for (auto& Pair : SVAttributeSet->TagsToAttributes)
	{
		SVAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			});
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	// 반드시 모든 바인드가 끝난 뒤에 호출해야 합니다.
	check(AttributeInfo);

	for (auto& Pair : SVAttributeSet->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	// Tag를 통해 Attribute의 Name, Description를 가져옵니다.
	FSVAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	// Tag에 일치하는 값은 직접 AttributeSet에서 찾아 가져옵니다.
	Info.AttributeValue = Attribute.GetNumericValue(SVAttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
