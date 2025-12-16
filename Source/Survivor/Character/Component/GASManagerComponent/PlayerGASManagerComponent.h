// KJY

#pragma once

#include "CoreMinimal.h"
#include "GASManagerComponent.h"
#include "PlayerGASManagerComponent.generated.h"

UCLASS()
class SURVIVOR_API UPlayerGASManagerComponent : public UGASManagerComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo() override;
	virtual void AddCharacterStartupAbilities() const override;
};
