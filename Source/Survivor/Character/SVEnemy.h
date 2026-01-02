// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "Survivor/Interface/EnemyInterface.h"
#include "SVEnemy.generated.h"

class UGameplayEffect;
class UFloatingPawnMovement;
class UGameplayAbility;
class UCapsuleComponent;
class UAttributeSet;
class UEnemyGASManagerComponent;

DECLARE_DELEGATE_OneParam(FOnEnemyDyingFinished, ASVEnemy*);

UCLASS()
class SURVIVOR_API ASVEnemy : public ASVCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	ASVEnemy(const FObjectInitializer& ObjectInitializer);

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~ End of APawn Interface

	//~ Begin ICombatInterface
	virtual void RegisterPawn(APawn* InPawn) override;
	virtual void UnregisterPawn(APawn* InPawn) override;
	//~ End of ICombatInterface

	virtual void OnSpawnFromPool() override;

	virtual void OnDeath() override;

private:
	void UpdateNearestTarget();

public:
	FOnEnemyDyingFinished OnEnemyDyingFinished;
	
private:
	TWeakObjectPtr<AActor> MoveTargetActor;
};
