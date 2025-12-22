// KJY

#include "ObjectPoolManagerSubsystem.h"

AActor* UObjectPoolManagerSubsystem::GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn)
{
	TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InClass).InactiveActors;
	
	// Pool 안에 객체 수가 부족하면 만들어서 반환합니다.
	if (ActorPool.IsEmpty())
	{
		bIsSpawning = true;

		const FTransform SpawnTransform = FTransform(PoolLocation);
		AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(
			InClass,
			SpawnTransform,
			OwnerActor,
			InstigatorPawn,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
			ESpawnActorScaleMethod::MultiplyWithRoot);
		return SpawnedActor;
	}

	// Pool 안에 객체가 있다면 그대로 반환합니다.
	bIsSpawning = false;
	return ActorPool.Pop();
}

void UObjectPoolManagerSubsystem::ReturnToPool(AActor* InActor)
{
	if (InActor)
	{
		InActor->SetActorHiddenInGame(true);
		InActor->SetActorEnableCollision(false);
		InActor->SetActorLocation(PoolLocation);
		TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InActor->GetClass()).InactiveActors;
		ActorPool.Emplace(InActor);
	}
}
