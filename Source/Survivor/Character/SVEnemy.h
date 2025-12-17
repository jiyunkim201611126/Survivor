// KJY

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Survivor/Interface/CombatInterface.h"
#include "Survivor/AI/SVAIController.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "SVEnemy.generated.h"

class UGameplayAbility;
class UCapsuleComponent;
class UAttributeSet;
class UEnemyGASManagerComponent;

UCLASS()
class SURVIVOR_API ASVEnemy : public APawn, public IAbilitySystemInterface, public ISVCameraAssistInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	ASVEnemy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~ Begin ICombatInterface
	virtual void ApplyKnockback(const FVector& KnockbackForce) override;
	//~ End of ICombatInterface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

public:
	UPROPERTY()
	TObjectPtr<ASVAIController> SVAIController;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyGASManagerComponent> GASManagerComponent;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Movement")
	float BaseWalkSpeed = 300.f;
};
