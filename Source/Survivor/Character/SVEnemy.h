// KJY

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Survivor/Interface/CombatInterface.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "SVEnemy.generated.h"

class UGameplayEffect;
class UFloatingPawnMovement;
class UGameplayAbility;
class UCapsuleComponent;
class UAttributeSet;
class UEnemyGASManagerComponent;

/**
 * Enemy는 그저 플레이어를 향해 걸어오기만 하는 아주 간단한 움직임만 필요합니다.
 * 따라서 CharacterMovementComponent처럼 무거운 클래스가 필요하지 않습니다.
 * 또한 장르 특성상 Enemy 수백 마리가 스폰되어 플레이어를 향해 오기 때문에, 이런 Enemy 최적화가 효과적일 거라 예상합니다.
 */
UCLASS()
class SURVIVOR_API ASVEnemy : public APawn, public IAbilitySystemInterface, public ISVCameraAssistInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ASVEnemy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UCapsuleComponent* GetCapsuleComponent() const;

	//~ Begin ICombatInterface
	virtual void ApplyKnockback(const FVector& KnockbackForce) override;
	//~ End of ICombatInterface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~ End of APawn Interface

	//~ Begin ISVCameraAssistInterface
	virtual void StartHide_Implementation() override;
	virtual void EndHide_Implementation() override;
	//~ End of ISVCameraAssistInterface

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlaySpawnAnimation();

private:
	void InitAbilityActorInfo() const;
	void UpdateNearestTarget();
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyGASManagerComponent> GASManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// CharacterMovementComponent보다 훨씬 가볍습니다.
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	TWeakObjectPtr<AActor> MoveTargetActor;
};
