// KJY

#include "ObjectPoolManagerSubsystem.h"

#include "Survivor/Character/SVEnemy.h"

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
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		return SpawnedActor;
	}

	// Pool 안에 객체가 있다면 그대로 반환합니다.
	bIsSpawning = false;
	AActor* PopActor = ActorPool.Pop();

	// Tick은 필요하지 않을 수 있으므로, 관리 객체 혹은 객체 자체가 켜서 사용합니다.
	PopActor->SetActorHiddenInGame(false);
	PopActor->SetActorEnableCollision(true);
	return PopActor;
}

void UObjectPoolManagerSubsystem::ReturnToPool(AActor* InActor)
{
	if (InActor)
	{
		TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InActor->GetClass()).InactiveActors;

		// Enemy 객체인 경우 Pool 안에 50개가 넘지 않도록 조정합니다.
		if (InActor->IsA(ASVEnemy::StaticClass()) && ActorPool.Num() > 50)
		{
			return;
		}

		// 게임 성능에 영향을 줄 수 있는 건 모두 끕니다.
		InActor->SetActorTickEnabled(false);
		InActor->SetActorHiddenInGame(true);
		InActor->SetActorEnableCollision(false);
		InActor->SetActorLocation(PoolLocation);
		
		ActorPool.Emplace(InActor);
	}
}
