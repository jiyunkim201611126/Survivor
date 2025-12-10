// KJY

#include "SVGameplayAbility.h"

#include "Survivor/Manager/SVGameplayTags.h"

void USVGameplayAbility::ApplyAllEffects(AActor* TargetActor)
{
	for (UGameplayEffectApplier* EffectApplier : EffectAppliers)
	{
		if (EffectApplier && TargetActor)
		{
			EffectApplier->ApplyEffect(this, TargetActor);
		}
	}
}

FGameplayEffectContextHandle USVGameplayAbility::GetContextHandle(const TSubclassOf<UGameplayEffectApplier> ApplierClass) const
{
	for (const auto EffectApplier : EffectAppliers)
	{
		if (EffectApplier && EffectApplier->GetClass() == ApplierClass)
		{
			return EffectApplier->GetEffectContextHandle();
		}
	}
	return FGameplayEffectContextHandle();
}

FText USVGameplayAbility::GetDescription_Implementation(const int32 Level)
{
	return FText();
}

float USVGameplayAbility::GetCooldown(const int32 InLevel) const
{
	return 0.f;
}

FText USVGameplayAbility::GetDamageTexts(const int32 InLevel) const
{
	return FText();
}

void USVGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	for (const auto EffectApplier : EffectAppliers)
	{
		EffectApplier->EndAbility();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


#if WITH_EDITOR
void USVGameplayAbility::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(USVGameplayAbility, AbilityTag))
	{
		SyncAbilityTagToAssetTags();
	}
}

void USVGameplayAbility::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		ActivationBlockedTags.AddTag(FSVGameplayTags::Get().CharacterState_Dead);
	}
}

void USVGameplayAbility::SyncAbilityTagToAssetTags()
{
	// GetAssetTags를 통해 반환받는 변수인 AbilityTags는 GameplayAbility의 멤버 변수입니다.
	// 추후 AssetTags라는 이름으로 변경될 예정이며, 메타데이터 역할을 수행하기 때문에 런타임 중 변경되는 것을 금지하고 있습니다.
	// 따라서 해당 함수는 에디터에서만 호출됩니다.
	if (AbilityTag.IsValid() && !GetAssetTags().HasTag(AbilityTag))
	{
		FGameplayTagContainer NewAbilityTags;
		NewAbilityTags.AddTag(AbilityTag);
		for (auto AssetTag : GetAssetTags())
		{
			NewAbilityTags.AddTag(AssetTag);
		}
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
				// AbilityTag에 값을 할당하면 자동으로 AbilityTags(AssetTags)에도 함께 할당해주는 구문입니다.
				// 에디터에서만 호출되기 때문에 안전합니다.
				AbilityTags = NewAbilityTags;
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
			}
}
#endif
