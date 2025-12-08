// KJY

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SVGameplayAbility.generated.h"

UCLASS()
class SURVIVOR_API USVGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AController* GetController() const;
};
