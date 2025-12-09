// KJY

#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "EnemyCombatComponent.generated.h"

UCLASS()
class SURVIVOR_API UEnemyCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo() override;
};
