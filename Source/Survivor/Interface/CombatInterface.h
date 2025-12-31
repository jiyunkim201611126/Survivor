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
	virtual void ApplyKnockback(const FVector& KnockbackForce) = 0;
	virtual UCapsuleComponent* GetCombatCapsuleComponent() const = 0;
};
