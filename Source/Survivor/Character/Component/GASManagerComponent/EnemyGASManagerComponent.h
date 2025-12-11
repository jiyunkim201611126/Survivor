// KJY

#pragma once

#include "CoreMinimal.h"
#include "GASManagerComponent.h"
#include "EnemyGASManagerComponent.generated.h"

UCLASS()
class SURVIVOR_API UEnemyGASManagerComponent : public UGASManagerComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo() override;
};
