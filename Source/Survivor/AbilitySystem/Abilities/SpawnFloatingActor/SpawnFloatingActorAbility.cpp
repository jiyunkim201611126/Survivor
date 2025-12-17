// KJY

#include "SpawnFloatingActorAbility.h"

#include "Survivor/AbilitySystem/SVAbilitySystemLibrary.h"
#include "FloatingActor.h"

void USpawnFloatingActorAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}
	
	const FVector OriginLocation = AvatarActor->GetActorLocation();
	FVector ForwardVector = AvatarActor->GetActorForwardVector();
	const float SpawnNums = SpawnNumsCurve.GetValueAtLevel(GetAbilityLevel());

	// Pool 안에 Actor 수가 부족하면 스폰해서 채워넣습니다.
	if (SpawnNums > FloatingActorPool.Num())
	{
		SpawnFloatingActors(SpawnNums - FloatingActorPool.Num());
	}

	// 위치를 계산한 뒤 Pool에서 가져와 그 위치에 놓습니다.
	const FRotator Rotation = AvatarActor->GetActorRotation();
	const TArray<FVector> SpawnLocations = USVAbilitySystemLibrary::CalculateEvenlyRotatedVectors(OriginLocation, ForwardVector, SpawnNums, SpawnAngle, SpawnLength);
	const float LifeTime = LifeTimeCurve.GetValueAtLevel(GetAbilityLevel());
	for (const FVector& SpawnLocation : SpawnLocations)
	{
		// 가장 마지막에 있는 Actor를 가져오며 Pool에서 제거합니다.
		AFloatingActor* CurrentActor = FloatingActorPool.Pop();
		
		CurrentActor->SetActorLocation(SpawnLocation);
		CurrentActor->SetActorRotation(Rotation);
		CurrentActor->SetLifeTime(LifeTime);
	}
}

void USpawnFloatingActorAbility::SpawnFloatingActors(const int32 SpawnNums)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	APawn* AvatarPawn = Cast<APawn>(AvatarActor);
	if (!AvatarActor || !AvatarPawn)
	{
		return;
	}

	for (int32 Index = 0; Index < SpawnNums; ++Index)
	{
		FTransform SpawnTransform = FTransform(FVector(0.f, 0.f, -10000.f));
		AFloatingActor* SpawnedFloatingActor = GetWorld()->SpawnActorDeferred<AFloatingActor>(
			SpawnActorClass,
			SpawnTransform,
			AvatarPawn,
			AvatarPawn,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			ESpawnActorScaleMethod::MultiplyWithRoot);
		SpawnedFloatingActor->OnFloatingActorActivateDamageDelegate.BindUObject(this, &ThisClass::OnFloatingActorActivateDamage);
		SpawnedFloatingActor->OnLifeEndDelegate.BindUObject(this, &ThisClass::OnLifeEnd);
		SpawnedFloatingActor->FinishSpawning(SpawnTransform);

		FloatingActorPool.Emplace(SpawnedFloatingActor);
	}
}

void USpawnFloatingActorAbility::OnFloatingActorActivateDamage(const TArray<AActor*>& TargetActors)
{
	for (AActor* TargetActor : TargetActors)
	{
		ApplyAllEffects(TargetActor);
	}
}

void USpawnFloatingActorAbility::OnLifeEnd(AFloatingActor* FloatingActor)
{
	FloatingActor->SetActorLocation(FVector(0.f, 0.f, -10000.f));
	FloatingActorPool.Emplace(FloatingActor);
}
