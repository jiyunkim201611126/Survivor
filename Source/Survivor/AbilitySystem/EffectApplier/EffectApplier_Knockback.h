// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectApplier.h"
#include "EffectApplier_Knockback.generated.h"

UCLASS()
class SURVIVOR_API UEffectApplier_Knockback : public UGameplayEffectApplier
{
	GENERATED_BODY()

public:
	virtual void ApplyEffect(UGameplayAbility* OwningAbility, AActor* TargetActor) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Knockback")
	float KnockbackChance = 100.f;

	// 피격 캐릭터의 속도(Velocity)를 변경하는 로직을 활용한다는 걸 고려해 수치를 입력하는 게 좋습니다.
	UPROPERTY(EditDefaultsOnly, Category = "Knockback")
	float KnockbackMagnitude = 300.f;
};
