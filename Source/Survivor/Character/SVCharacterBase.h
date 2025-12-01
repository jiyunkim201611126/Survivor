// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Survivor/Interface/CombatInterface.h"
#include "SVCharacterBase.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class SURVIVOR_API ASVCharacterBase : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	ASVCharacterBase();

protected:
	virtual void InitAbilityActorInfo();

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	FOnASCRegistered OnASCRegistered;
};
