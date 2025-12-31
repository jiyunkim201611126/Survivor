// KJY

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UINTERFACE()
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

class SURVIVOR_API IEnemyInterface
{
	GENERATED_BODY()

public:
	virtual void OnSpawnFromPool() = 0;
};
