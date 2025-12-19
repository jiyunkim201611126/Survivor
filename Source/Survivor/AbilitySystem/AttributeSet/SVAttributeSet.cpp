// KJY

#include "SVAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Survivor/Interface/CombatInterface.h"
#include "Survivor/SVAbilityTypes.h"
#include "Survivor/AbilitySystem/SVAbilitySystemLibrary.h"
#include "Survivor/Manager/DamageSlateManagerSubsystem.h"
#include "Survivor/Manager/SVGameplayTags.h"

USVAttributeSet::USVAttributeSet()
{
	const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();

	TagsToAttributes.Reserve(4);
	
	TagsToAttributes.Emplace(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Attack, GetAttackAttribute);
	TagsToAttributes.Emplace(GameplayTags.Attributes_Stat_Armor, GetArmorAttribute);
}

void USVAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USVAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USVAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USVAttributeSet, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USVAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}

void USVAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// TODO: Source가 사망한 경우 return

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		ApplyIncomingDamage(Props, Data);
	}
}

void USVAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void USVAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const
{
	// GE가 적용되는 상황에서 ASC, SourceAvatarActor, TargetAvatarActor등을 편리하게 추적하기 위해 사용하는 함수입니다. 
	EffectProperties.EffectContextHandle = Data.EffectSpec.GetContext();
	EffectProperties.SourceASC = EffectProperties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(EffectProperties.SourceASC) && EffectProperties.SourceASC->AbilityActorInfo.IsValid() && EffectProperties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.SourceAvatarActor = EffectProperties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		EffectProperties.SourceController = EffectProperties.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (EffectProperties.SourceController == nullptr && EffectProperties.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(EffectProperties.SourceAvatarActor))
			{
				EffectProperties.SourceController = Pawn->GetController();
			}
		}
		if (EffectProperties.SourceController)
		{
			EffectProperties.SourceCharacter = Cast<ACharacter>(EffectProperties.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		EffectProperties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		EffectProperties.TargetCharacter = Cast<ACharacter>(EffectProperties.TargetAvatarActor);
		EffectProperties.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(EffectProperties.TargetAvatarActor);
	}
}

void USVAttributeSet::ApplyIncomingDamage(const FEffectProperties& Props, const FGameplayEffectModCallbackData& Data)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	if (!Props.TargetAvatarActor || !Props.SourceAvatarActor)
	{
		return;
	}

	// 넉백 방향 계산 및 Damage Text 위치를 계산하기 위해 두 액터의 위치를 가져옵니다.
	FVector TargetLocation = Props.TargetAvatarActor->GetActorLocation();
	FVector SourceLocation = Props.SourceAvatarActor->GetActorLocation();
	const FVector DamageTextLocation = (TargetLocation + SourceLocation) / 2.f;

	float DamageForText = LocalIncomingDamage;
	if (LocalIncomingDamage > 0.01f)
	{
		// 새로운 체력을 계산해 할당합니다.
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		// 유효한 데미지가 적용되었으므로, 넉백을 적용합니다.
		const FDamageDataContext DamageData = USVAbilitySystemLibrary::GetDamageData(Props.EffectContextHandle);
		const float KnockbackMagnitude = DamageData.KnockbackMagnitude;
		if (!FMath::IsNearlyZero(KnockbackMagnitude))
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor))
			{
				TargetLocation.Z = 0;
				SourceLocation.Z = 0;
				const FVector HorizontalDirection = (TargetLocation - SourceLocation).GetSafeNormal();
				const FVector KnockbackDirection = (HorizontalDirection + FVector::UpVector * 0.5f) * KnockbackMagnitude;
				CombatInterface->ApplyKnockback(KnockbackDirection);
			}
		}
		
		const bool bFatal = NewHealth <= 0.f;
		if (bFatal)
		{
			// TODO: 사망 처리 및 XP Event 송신
		}
	}
	else
	{
		// 데미지가 0.01보다 작으면 체력 감소를 적용하지 않습니다.
		SetIncomingDamage(0.f);
		DamageForText = 0.f;
	}

	// 두 액터 위치 사이에 데미지 텍스트를 표시합니다.
	GetWorld()->GetSubsystem<UDamageSlateManagerSubsystem>()->ShowDamageNumber(DamageForText, DamageTextLocation);
	
	// TODO: 적인 경우 ProgressBar Slate 사용
}

void USVAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USVAttributeSet, Health, OldHealth);
}

void USVAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USVAttributeSet, MaxHealth, OldMaxHealth);
}

void USVAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USVAttributeSet, Attack, OldAttack);
}

void USVAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USVAttributeSet, Armor, OldArmor);
}
