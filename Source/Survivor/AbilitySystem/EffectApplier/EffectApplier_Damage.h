// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectApplier.h"
#include "ScalableFloat.h"
#include "EffectApplier_Damage.generated.h"

UCLASS()
class SURVIVOR_API UEffectApplier_Damage : public UGameplayEffectApplier
{
	GENERATED_BODY()

public:
	virtual void ApplyEffect(UGameplayAbility* OwningAbility, AActor* TargetActor) override;

	TArray<FGameplayEffectSpecHandle> MakeDamageSpecHandle(const UGameplayAbility* OwningAbility);
	void CauseDamage(const UGameplayAbility* OwningAbility, AActor* TargetActor, const TArray<FGameplayEffectSpecHandle>& DamageSpecs);
	virtual FText GetDamageDescription(int32 InLevel);

private:
	// 데미지 타입과 그 속성 데미지를 정의하는 변수입니다.
	// 여기서 데미지는 EffectApplier 고유의 데미지로, 최종적으로는 Attack Attribute가 추가로 붙어서 가해집니다.
	// 전혀 할당하지 않는 경우 Attack Attribute도 계산하지 않게 되므로, 하나 이상의 DamageType을 가져야 합니다.
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
