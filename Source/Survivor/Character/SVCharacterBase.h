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

UCLASS()
class SURVIVOR_API ASVCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface, public ISVCameraAssistInterface
{
	GENERATED_BODY()

public:
	ASVCharacterBase(const FObjectInitializer& ObjectInitializer);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	//~ Begin ICombatInterface
	virtual void ApplyKnockback(const FVector& KnockbackForce) override;
	//~ End of ICombatInterface

protected:
	void InitAbilityActorInfo() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGASManagerComponent> GASManagerComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
