// KJY

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE()
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVOR_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void ApplyKnockback(const FVector_NetQuantize& KnockbackForce, const float Duration);
};
