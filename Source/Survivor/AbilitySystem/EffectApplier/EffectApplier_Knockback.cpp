// KJY

#include "EffectApplier_Knockback.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Survivor/AbilitySystem/SVAbilitySystemLibrary.h"
#include "Survivor/Interface/CombatInterface.h"

void UEffectApplier_Knockback::ApplyEffect(UGameplayAbility* OwningAbility, AActor* TargetActor)
{
	// TODO: 넉백 면역 태그 소지 시 로직을 실행하지 않습니다.
	
	// 넉백은 확률 계산 후 성공 시 적용합니다.
	if (FMath::FRandRange(0.f, 100.f) >= KnockbackChance)
	{
		return;
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	UAbilitySystemComponent* SourceASC = OwningAbility->GetAbilitySystemComponentFromActorInfo();
	
	if (!FMath::IsNearlyZero(KnockbackMagnitude))
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetActor))
		{
			// 두 AvatarActor의 위치를 토대로 넉백 방향과 힘의 크기를 계산한 뒤, KnockbackResistance까지 계산해서 적용합니다.
			FVector TargetLocation = TargetActor->GetActorLocation();
			FVector SourceLocation = OwningAbility->GetAvatarActorFromActorInfo()->GetActorLocation();
			TargetLocation.Z = 0;
			SourceLocation.Z = 0;
			const FVector HorizontalDirection = (TargetLocation - SourceLocation).GetSafeNormal();
			FVector KnockbackForce = (HorizontalDirection + FVector::UpVector * 0.5f) * KnockbackMagnitude;
			KnockbackForce *= 1.0f - TargetASC->GetNumericAttribute(USVAttributeSet::GetKnockbackResistanceAttribute());
			CombatInterface->ApplyKnockback(KnockbackForce);
		}
	}
		
	// 할당받은 KnockbackEffectClass를 기반으로 GameplayEffectSpec을 생성합니다.
	const FGameplayEffectSpecHandle KnockbackSpecHandle = SourceASC->MakeOutgoingSpec(EffectClass, 1.f, EffectContextHandle);
	
	MakeEffectContextHandle(OwningAbility);
	if (EffectContextHandle.IsValid())
	{
		// 대상을 관련 액터에 추가합니다.
		TArray<TWeakObjectPtr<AActor>> TargetActors;
		TargetActors.Add(TargetActor);
		EffectContextHandle.AddActors(TargetActors);
	}

	// Knockback 태그 부여를 위해 Effect를 적용합니다.
	if (SourceASC && TargetASC)
	{
		SourceASC->ApplyGameplayEffectSpecToTarget(*KnockbackSpecHandle.Data.Get(), TargetASC);
	}
}
