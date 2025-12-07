// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVWidgetController.h"
#include "OverlayWidgetController.generated.h"

class USVUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AssetTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<USVUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChangedSignature, int32, InCurrentXP, int32, InMaxXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelChangedSignature, int32, NewLevel, bool, bLevelUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

UCLASS(Abstract, BlueprintType, Blueprintable)
class SURVIVOR_API UOverlayWidgetController : public USVWidgetController
{
	GENERATED_BODY()

public:
	//~ Begin AuraWidgetController Interface
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End AuraWidgetController Interface

	UFUNCTION(BlueprintCallable)
	void OnPlayButtonClicked() const;

private:
	void OnXPChanged(int32 InXP);

	// Message 태그를 가진 GE가 적용되면 호출되는 함수입니다.
	void OnEffectHasMessageTagApplied(const FGameplayTagContainer& GameplayTags) const;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChangedSignature OnShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FOnAttributeChangedSignature OnMaxShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS | XP")
	FOnXPChangedSignature OnXPChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Level")
	FOnLevelChangedSignature OnPlayerLevelChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
};
