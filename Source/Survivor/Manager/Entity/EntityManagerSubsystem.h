// KJY

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EntityManagerSubsystem.generated.h"

class UNavigationSystemV1;
class ASVEnemy;
class AEntitySpawner;

USTRUCT()
struct FEntityInstanceData
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TObjectPtr<APawn> Target;

	UPROPERTY()
	TArray<FVector> PathPoints;

	UPROPERTY()
	int32 CurrentPathIndex = 0;
};

USTRUCT()
struct FEntityPool
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> InstancedStaticMeshComponent;

	// 각 인스턴스의 데이터(Transform, Target)를 저장하는 배열입니다.
	// InstancedStaticMeshComponent의 Index를 기반으로 아래 배열의 요소와 매핑되기 때문에, Index 관리가 매우 중요합니다.
	UPROPERTY()
	TArray<FEntityInstanceData> Instances;
};

UCLASS(Config = Game)
class SURVIVOR_API UEntityManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin FTickableGameObject Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~ End of FTickableGameObject Interface

	// EntitySpawner가 EQS를 통해 랜덤 위치를 생성한 뒤 이 함수를 호출합니다.
	void SpawnEntities(const uint8 MonsterID, const TArray<FVector>& NewLocations, APawn* TargetPawn);

private:
	void InitEntitySpawner();

	void MoveToTargetWithNavMesh(FEntityInstanceData& InstanceData, const UNavigationSystemV1* NavSystem, APawn* TargetPawn, const float DeltaTime, const float MonsterSpeed, const float WaypointArrivalThresholdSquared);
	
	// InstancedMesh와 매핑된 PlayerPawn을 가져오는 함수입니다.
	APawn* GetEntityTarget(const FEntityPool& Pool, const int32 InstanceIndex) const;

private:
	// uint8은 MonsterID를 의미합니다.
	UPROPERTY()
	TMap<uint8, FEntityPool> EntityPools;

	UPROPERTY()
	TMap<uint8, TObjectPtr<ASVEnemy>> EnemyPools;

	UPROPERTY(Config)
	TSubclassOf<AEntitySpawner> EntitySpawnerClass;

	UPROPERTY()
	TObjectPtr<AEntitySpawner> GlobalEntitySpawner;

	float RefreshNavPathThreshold = 0.5f;
	float RefreshNavPathTime = 0.f;
};
