// KJY

#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "PlayerCombatComponent.generated.h"

UCLASS()
class SURVIVOR_API UPlayerCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo() override;
};
