// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Survivor/Interface/CombatInterface.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "SVCharacterBase.generated.h"

class UGASManagerComponent;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
class SURVIVOR_API ASVCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface, public ISVCameraAssistInterface
{
	GENERATED_BODY()

public:
	ASVCharacterBase(const FObjectInitializer& ObjectInitializer);
	
	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End of IAbilitySystemInterface
	
	//~ Begin ICombatInterface
	virtual UCapsuleComponent* GetCombatCapsuleComponent() const override;
	virtual void ApplyKnockback(const FVector& KnockbackForce) override;
	virtual void Die() override;
	//~ End of ICombatInterface

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastDeath();

protected:
	void InitAbilityActorInfo() const;

	void StartOnDeathTimer();
	virtual void OnDeath();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGASManagerComponent> GASManagerComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
