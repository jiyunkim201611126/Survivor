// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FSVAttributeInfo&, Info);

UCLASS(Abstract, BlueprintType, Blueprintable)
class SURVIVOR_API UAttributeMenuWidgetController : public USVWidgetController
{
	GENERATED_BODY()

public:
	//~ Begin AuraWidgetController Interface
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValue() override;
	//~ End AuraWidgetController Interface

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "GAS | Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
};
