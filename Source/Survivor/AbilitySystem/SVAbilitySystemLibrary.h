// KJY

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet/SVAttributeSet.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Survivor/SVAbilityTypes.h"
#include "SVAbilitySystemLibrary.generated.h"

struct FWidgetControllerParams;
class ASVHUD;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;

UCLASS()
class SURVIVOR_API USVAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | WidgetController", meta = (WorldContext = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, ASVHUD*& OutSVHUD);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | WidgetController", meta = (WorldContext = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | WidgetController", meta = (WorldContext = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | GameplayEffects")
	static FDamageDataContext GetDamageData(const FGameplayEffectContextHandle& EffectContextHandle);
};
