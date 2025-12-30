// KJY

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolManagerSubsystem.generated.h"

class AFloatingActor;

/**
 * TMap의 Value에는 TArray 자료형이 들어갈 수 없으므로, 구조체로 감싸 우회합니다.
 */
USTRUCT()
struct FActorPoolContainer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> InactiveActors;
};

/**
 * 생성과 파괴가 빠른 빈도로 반복되어야 하는 경우 사용하는 ObjectPool 패턴의 매니저 클래스입니다.
 * 생성한 뒤 필요할 때 꺼내 쓰는 것으로 최적화합니다.
 */
UCLASS()
class SURVIVOR_API UObjectPoolManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 플레이에 지장을 주지 못 하도록 Pool 시스템에 의해 초기화되는 위치입니다.
	inline static const FVector PoolLocation = FVector(0.f, 0.f, -10000.f);

	template<typename T>
	T* GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn);
	AActor* GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn);
	
	void ReturnToPool(AActor* InActor);

private:
	// Class를 Key로 Pool을 가져와 꺼내 사용할 수 있습니다.
	UPROPERTY()
	TMap<TSubclassOf<AActor>, FActorPoolContainer> ActorPoolMap;
};

template <typename T>
T* UObjectPoolManagerSubsystem::GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn)
{
	return Cast<T>(GetFromPool(InClass, bIsSpawning, OwnerActor, InstigatorPawn));
}
