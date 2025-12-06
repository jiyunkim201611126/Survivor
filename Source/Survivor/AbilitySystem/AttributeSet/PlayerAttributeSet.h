// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVAttributeSet.h"
#include "PlayerAttributeSet.generated.h"

UCLASS()
class SURVIVOR_API UPlayerAttributeSet : public USVAttributeSet
{
	GENERATED_BODY()

public:
	UPlayerAttributeSet();
	
	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface
	
	//~ Begin UAttributeSet Interface
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	//~ End UAttributeSet Interface

public:
	
	/**
	* Meta Attributes
	*/
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingXP);
};
