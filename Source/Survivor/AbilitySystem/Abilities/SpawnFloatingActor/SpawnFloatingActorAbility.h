// KJY

#pragma once

#include "CoreMinimal.h"
#include "Survivor/AbilitySystem/Abilities/SVGameplayAbility.h"
#include "SpawnFloatingActorAbility.generated.h"

class AFloatingActor;

/**
 * PlayerCharacter 전용 Ability로 파생되는 Base 클래스입니다.
 * Enemy Ability의 경우 Cooldown GameplayEffect를 사용하는 GAS의 방식을 그대로 따르지만,
 * PlayerCharacter의 Ability들은 게임 후반부에 굉장히 빠른 속도로 발동되기 때문에, GAS의 방식을 그대로 따르면 최적화 문제가 발생합니다.
 * 따라서 한 번만 발동 후 EndAbility를 호출하지 않으며, 반복적으로 Actor를 스폰시키도록 구성합니다.
 */
UCLASS()
class SURVIVOR_API USpawnFloatingActorAbility : public USVGameplayAbility
{
	GENERATED_BODY()

protected:
	//~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~ End of UGameplayAbility Interface
	
private:
	UFUNCTION()
	void LoopAbility();
	void SpawnFloatingActors();
	
	void OnFloatingActorActivateDamage(const TArray<AActor*>& TargetActors);
	void OnLifeEnd(AActor* InActor) const;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AFloatingActor> SpawnActorClass;

	UPROPERTY(EditAnywhere)
	FScalableFloat SpawnNumsCurve;

	UPROPERTY(EditAnywhere)
	FScalableFloat SpawnAngleCurve;

	UPROPERTY(EditAnywhere)
	float SpawnLength = 300.f;

	UPROPERTY(EditAnywhere)
	FScalableFloat LifeTimeCurve;

	// Cooldown Effect를 사용하지 않기 때문에 Ability 내에서 직접 Curve를 사용해 레벨당 쿨다운을 구현합니다.
	UPROPERTY(EditAnywhere)
	FScalableFloat CooldownCurve;
};
