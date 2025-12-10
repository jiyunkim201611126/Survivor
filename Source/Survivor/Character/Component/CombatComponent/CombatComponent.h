// KJY

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/PawnComponent.h"
#include "CombatComponent.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract, NotBlueprintable)
class SURVIVOR_API UCombatComponent : public UPawnComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UCombatComponent(const FObjectInitializer& ObjectInitializer);

	// PlayerCharacter는 초기화 시점에 외부에서 두 객체를 넘겨받지만, Enemy의 경우 캐릭터쪽에서 직접 생성하기 때문에, 넘겨받아 할당합니다.
	void SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent);
	void SetAttributeSet(UAttributeSet* InAttributeSet);
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void InitAbilityActorInfo();
	void AddCharacterStartupAbilities() const;
	
protected:
	// GameplayEffect를 본인에게 적용하는 함수입니다.
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;

public:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	// 게임 시작 시 부여되는 Ability입니다.
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Hide")
	TArray<TObjectPtr<UMaterialInstance>> FadeMaterials;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Hide")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> OriginalMaterialInstances;

	UPROPERTY(BlueprintReadOnly, Category = "Hide")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> FadeMaterialInstances;
};
