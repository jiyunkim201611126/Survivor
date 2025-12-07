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

public:

	/**
	 * Stat Attributes
	 *
	 * AttributeMenu에 표시를 원하는 경우, 아래 순서대로 진행해주세요.
	 * 1. TagsToAttributes에 키-값 쌍 추가
	 * 2. AttributeMenu 위젯에서 Attribute 위젯 추가 및 Tag 할당
	 * 3. DA_AttributeInfo에 해당 Attribute 정보 추가
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Stat Attributes")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Shield);
	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "Stat Attributes")
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxShield);
	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AdditionalSpeed, Category = "Stat Attributes")
	FGameplayAttributeData AdditionalSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AdditionalSpeed);
	UFUNCTION()
	void OnRep_AdditionalSpeed(const FGameplayAttributeData& OldAdditionalSpeed) const;
	
	/**
	 * Meta Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingXP);
};
