// KJY

#pragma once

#include "CoreMinimal.h"
#include "GASManagerComponent.h"
#include "PlayerGASManagerComponent.generated.h"

struct FGameplayTag;

UCLASS()
class SURVIVOR_API UPlayerGASManagerComponent : public UGASManagerComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo() override;
	virtual void AddCharacterStartupAbilities() const override;

private:
	void OnCooldownTagChanged(const FGameplayTag CooldownTag, const int32 NewCount) const;
};
