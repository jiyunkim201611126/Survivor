// KJY

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SVAttributeSet.generated.h"

// Getter, Setter, Initter를 자동 생성해주는 매크로를 호출하기 위해 정의하는 구문입니다.
// 이 구문이 없으면 Getter, Setter, Initter를 변수마다 모두 작성해야 합니다.
// 즉 Boilerplate 코드를 줄여주는 구문입니다.
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// 원문 그대로 사용하면 너무 추악하기 때문에 한 번 이쁘게 포장합니다.
template<class T>
using TStaticFuncPtr = TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

// Attribute에게 변화가 적용되는 모든 상황에 대해서 Source와 Target을 추적하기 위해 선언, 초기화하는 구조체입니다.
// 사실상 편의성을 위해 사용되는 구조체입니다.
USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

UCLASS()
class SURVIVOR_API USVAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	USVAttributeSet();
	
	//~ Begin UObject Interface
	// 어떤 변수들이 Replicate될지, 어떻게 Replicate될지 지정하는 함수입니다.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface
	
	//~ Begin UAttributeSet Interface
	// GameplayEffect의 적용으로 인해 Attribute에 변동사항이 있으면 호출되는 함수입니다.
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	// Attribute의 값이 변화할 때 호출되는 함수입니다.
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	//~ End UAttributeSet Interface

private:
	// GE 적용 시점에 Source와 Target을 편리하게 추적하기 위해 구조체에 그 정보를 채워주는 함수입니다.
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const;

public:
	// 델리게이트가 아닌 함수 포인터를 직접 Value로 선언한 TMap.
	// 저수준 방식으로 최적화를 위해서 사용하기도 하지만, Bind 과정을 거칠 필요가 없기 때문에 보일러 플레이트를 줄이는 데에도 사용 가능합니다.
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

	/**
	 * Vital Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USVAttributeSet, Health);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USVAttributeSet, MaxHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	/**
	 * Stat Attributes
	*/
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "Stat Attributes")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(USVAttributeSet, Attack);
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Stat Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(USVAttributeSet, Armor);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	/**
	 * Meta Attributes
	 * 
	 * 값 변화가 있을 때 위에서 선언한 Attribute에 바로 적용하는 게 아니라 중간다리 역할을 해주는 Attribute를 선언해 사용합니다.
	 * Meta Attribute는 복제되지 않습니다.
	 * 
	 * Meta Attribute 없이 직접 적용하는 방식은 각 효과의 순서가 매우 중요해집니다.
	 * 예를 들어 데미지 경감 효과를 나중에 계산한다면 캐릭터의 Health가 양수임에도 잠시 0 이하로 내려가는 상황이 발생, 캐릭터가 의도치 않게 사망할 수 있습니다.
	 * Meta Attribute를 사용하면 여러 효과(데미지 경감, 추가 피해 등)가 동시에 적용될 때 로직 순서에 덜 신경 써도 되며
	 * 중복 적용, 누락 등의 문제를 자연스럽게 방지할 수 있습니다.
	 */
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(USVAttributeSet, IncomingDamage);
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(USVAttributeSet, IncomingXP);
};
