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
	void SpawnFloatingActors(int32 SpawnNums);
	void OnFloatingActorActivateDamage(const TArray<AActor*>& TargetActors);
	void OnLifeEnd(AFloatingActor* FloatingActor);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AFloatingActor> SpawnActorClass;

	UPROPERTY(EditAnywhere)
	FScalableFloat SpawnNumsCurve;

	UPROPERTY(EditAnywhere)
	float SpawnAngle = 180.f;

	UPROPERTY(EditAnywhere)
	float SpawnLength = 300.f;

	UPROPERTY(EditAnywhere)
	FScalableFloat LifeTimeCurve;

	UPROPERTY()
	TArray<TObjectPtr<AFloatingActor>> FloatingActorPool;
};
