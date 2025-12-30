// KJY

#pragma once

#include "CoreMinimal.h"
#include "GASManagerComponent.h"
#include "EnemyGASManagerComponent.generated.h"

struct FGameplayTag;

UCLASS()
class SURVIVOR_API UEnemyGASManagerComponent : public UGASManagerComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo() override;
	
	void OnOwnerSpawnFromPool() const;

private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnCooldownTagChanged(const FGameplayTag CooldownTag, const int32 NewCount) const;
};
