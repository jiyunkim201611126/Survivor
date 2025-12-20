// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "SVEnemy.generated.h"

class UGameplayEffect;
class UFloatingPawnMovement;
class UGameplayAbility;
class UCapsuleComponent;
class UAttributeSet;
class UEnemyGASManagerComponent;

UCLASS()
class SURVIVOR_API ASVEnemy : public ASVCharacterBase
{
	GENERATED_BODY()

public:
	ASVEnemy(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~ End of APawn Interface

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlaySpawnAnimation();

private:
	void UpdateNearestTarget();
	
private:
	TWeakObjectPtr<AActor> MoveTargetActor;
};
