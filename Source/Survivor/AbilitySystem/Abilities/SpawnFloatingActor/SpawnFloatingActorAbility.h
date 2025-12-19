// KJY

#pragma once

#include "CoreMinimal.h"
#include "Survivor/AbilitySystem/Abilities/SVGameplayAbility.h"
#include "SpawnFloatingActorAbility.generated.h"

class AFloatingActor;

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
