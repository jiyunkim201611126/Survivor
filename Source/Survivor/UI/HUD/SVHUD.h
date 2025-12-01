// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SVHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class SURVIVOR_API ASVHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitHUD(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
};
