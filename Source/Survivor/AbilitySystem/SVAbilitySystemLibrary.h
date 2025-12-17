// KJY

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet/SVAttributeSet.h"
#include "EffectApplier/EffectApplier_Damage.h"
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
	
	static void SetKnockbackMagnitude(FGameplayEffectContextHandle& EffectContextHandle, const float Magnitude);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary | GameplayEffects")
	static TArray<AActor*> GetActorsFromContext(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * ForwardVector를 기준으로 총 Degree만큼 펼쳐지고 Length만큼 해당 방향을 향해 나아간 NumberOfVectors개의 Vectors(Locations)를 반환합니다.
	 * 
	 * @param OriginLocation 기준이 되는 위치입니다.
	 * @param NormalizedForwardVector 기준이 되는 정면 Vector입니다. 단위 벡터를 사용하며, Actor의 Forward Vector 등을 사용할 수 있습니다.
	 * @param NumOfVectors 반환할 Vector의 개수입니다.
	 * @param Angle 펼쳐질 총 각도입니다.
	 * @param Length 앞으로 나아갈 거리입니다.
	 * @return 결과 벡터들을 반환합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary | HelperFunctions")
	static TArray<FVector> CalculateEvenlyRotatedVectors(UPARAM(ref) const FVector& OriginLocation, UPARAM(ref) FVector& NormalizedForwardVector, const int32 NumOfVectors, const float Angle, const float Length);
};
