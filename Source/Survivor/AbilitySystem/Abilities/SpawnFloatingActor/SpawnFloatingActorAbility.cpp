// KJY

#include "SpawnFloatingActorAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Survivor/AbilitySystem/SVAbilitySystemLibrary.h"
#include "FloatingActor.h"
#include "Survivor/Manager/ObjectPoolManagerSubsystem.h"
#include "Survivor/Manager/SVGameplayTags.h"

void USpawnFloatingActorAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 최적화를 위해 EndAbility를 호출하지 않고, Active 한 번 안에서 반복합니다.
	// 이렇게 하면 Ability 사용이 반복적으로 Replicate되지 않으며, Ability 객체가 파괴되지 않는 이점도 가질 수 있습니다.
	LoopAbility();
}

void USpawnFloatingActorAbility::LoopAbility()
{
	if (!GetAbilitySystemComponentFromActorInfo() || GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FSVGameplayTags::Get().CharacterState_Dead))
	{
		return;
	}
	
	SpawnFloatingActors();

	const float NextCooldown = CooldownCurve.GetValueAtLevel(GetAbilityLevel());

	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, NextCooldown);
	DelayTask->OnFinish.AddDynamic(this, &ThisClass::LoopAbility);
	DelayTask->ReadyForActivation();
}

void USpawnFloatingActorAbility::SpawnFloatingActors()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	APawn* AvatarPawn = Cast<APawn>(AvatarActor);
	if (!AvatarActor || !AvatarPawn || !GetWorld())
	{
		return;
	}
	
	// Actor의 위치를 계산합니다.
	const FVector ActorLocation = AvatarActor->GetActorLocation();
	FVector ForwardVector = AvatarActor->GetActorForwardVector();
	const float SpawnNums = SpawnNumsCurve.GetValueAtLevel(GetAbilityLevel());
	const float SpawnAngle = SpawnAngleCurve.GetValueAtLevel(GetAbilityLevel());
	
	TArray<FVector> SpawnLocations;
	USVAbilitySystemLibrary::CalculateEvenlyRotatedVectors(ForwardVector, SpawnNums, SpawnAngle, SpawnLength, SpawnLocations);

	// Pool에서 꺼내와 해당 위치로 이동시킵니다.
	if (UObjectPoolManagerSubsystem* ObjectPoolManager = GetWorld()->GetGameInstance()->GetSubsystem<UObjectPoolManagerSubsystem>())
	{
		for (const FVector& SpawnLocation : SpawnLocations)
		{
			bool bIsSpawning;
			AFloatingActor* CurrentActor = ObjectPoolManager->GetFromPool<AFloatingActor>(SpawnActorClass, bIsSpawning, AvatarActor, AvatarPawn);
			if (bIsSpawning)
			{
				// 새로 스폰하는 경우 함수를 바인드하고 FinishSpawning을 호출합니다.
				CurrentActor->OnFloatingActorActivateDelegate.BindUObject(this, &ThisClass::ApplyAllEffects);
				CurrentActor->OnLifeEndDelegate.BindUObject(this, &ThisClass::OnLifeEnd);
				CurrentActor->FinishSpawning(FTransform());
			}

			CurrentActor->SetOwner(AvatarActor);
			CurrentActor->SetLifeTime(LifeTimeCurve.GetValueAtLevel(GetAbilityLevel()));
			CurrentActor->Activate(SpawnLocation + ActorLocation, SpawnLocation.Rotation());
		}
	}
}

void USpawnFloatingActorAbility::OnLifeEnd(AFloatingActor* InFloatingActor) const
{
	// 스폰한 Actor의 LifeTime이 종료되면 일부 데이터를 리셋하고 다시 Pool에 반환합니다.
	InFloatingActor->ResetOverlapActors();
	if (GetWorld())
	{
		if (UObjectPoolManagerSubsystem* ObjectPoolManager = GetWorld()->GetGameInstance()->GetSubsystem<UObjectPoolManagerSubsystem>())
		{
			ObjectPoolManager->ReturnToPool(InFloatingActor);
		}
	}
}
