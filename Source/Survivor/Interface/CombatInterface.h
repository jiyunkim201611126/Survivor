// KJY

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UCapsuleComponent;

UINTERFACE()
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVOR_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void RegisterPawn(APawn* InPawn);
	virtual void UnregisterPawn(APawn* InPawn);
	
	virtual UCapsuleComponent* GetCombatCapsuleComponent() const = 0;
	virtual void ApplyKnockback(const FVector& KnockbackForce) = 0;
	virtual void Die() = 0;
};
