// KJY

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "Survivor/Interface/CombatInterface.h"
#include "SVCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class SURVIVOR_API ASVCharacterBase : public ACharacter, public ICombatInterface, public IAbilitySystemInterface, public ISVCameraAssistInterface
{
	GENERATED_BODY()

public:
	ASVCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual void InitAbilityActorInfo();

	// GameplayEffect를 본인에게 적용하는 함수입니다.
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const;
	virtual void AddCharacterStartupAbilities() const;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	FOnASCRegistered OnASCRegistered;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultAttributes;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hide")
	TArray<TObjectPtr<UMaterialInstance>> FadeMaterials;
	
	UPROPERTY(BlueprintReadOnly, Category = "Hide")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> OriginalMaterialInstances;

	UPROPERTY(BlueprintReadOnly, Category = "Hide")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> FadeMaterialInstances;
};
