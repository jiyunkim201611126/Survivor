// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32, NewLevel, bool, bLevelUp);

UCLASS(Abstract, BlueprintType, Blueprintable)
class SURVIVOR_API UOverlayWidgetController : public USVWidgetController
{
	GENERATED_BODY()

public:
	//~ Begin AuraWidgetController Interface
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End AuraWidgetController Interface

private:
	void OnXPChanged(int32 InXP);
	
public:
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | XP")
	FOnAttributeChangedSignature OnXPBarPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Level")
	FOnLevelChangedSignature OnPlayerLevelChangedDelegate;
};
