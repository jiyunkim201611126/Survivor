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

/**
 * Entity들의 Target, 목적지, 현재 위치 등을 총괄하는 매니저 클래스입니다.
 * 게임 특성상 수백 마리의 적이 나타나기 때문에, 이를 모두 Actor로 처리하면 엄청난 부하가 발생합니다.
 * 따라서 플레이어와 멀리 떨어져 GAS 상호작용이 불필요한 적은 Entity로 처리하기로 결정해 구현하게 된 클래스입니다.
 * InstancedStaticMeshComponent를 통해 사각형 하나에 텍스쳐를 씌운 Mesh 여러 개를 적 위치에 생성, DrawCall 1회로 다수의 적을 보여줍니다.
 * 해당 클래스에서 Transform을 계산하는데, 이를 Material이 덮어씌워 다시 렌더링합니다.
 * Mesh는 항상 카메라를 바라보며, Rotation으로 받은 값을 토대로 카메라 방향 벡터와 내적/외적, 그 값으로 텍스쳐를 갈아 끼우는 작업을 Material이 수행합니다.
 * 이를 통해 CPU 최적화를 극한으로 이루어냈(다고 생각하고 있)습니다.
 */
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

	// Entity가 NavMesh 기반으로 Target을 향해 움직일 때, 다음 프레임의 위치를 가져오는 함수입니다.
	FVector GetNextLocationWithNavMesh(FEntityInstanceData& InstanceData, const UNavigationSystemV1* NavSystem, APawn* TargetPawn, const float DeltaTime, const float MonsterSpeed, const float WaypointArrivalThresholdSquared);

	// Entity가 서로 겹치지 않도록 어느 방향으로 밀어낼지 계산하는 함수입니다.
	FVector CalculateSeparationOffset(const FEntityInstanceData* CurrentInstance, const TMap<FIntVector, TArray<FEntityInstanceData*>>& EntityGrid) const;
	
	// InstancedMesh와 매핑된 PlayerPawn을 가져오는 함수입니다.
	APawn* GetEntityTarget(const FEntityPool& Pool, const int32 InstanceIndex) const;

private:
	// uint8은 MonsterID를 의미합니다.
	UPROPERTY()
	TMap<uint8, FEntityPool> EntityPools;

	int32 MaxEntityCount = 500;

	UPROPERTY()
	TMap<uint8, TObjectPtr<ASVEnemy>> EnemyPools;

	UPROPERTY(Config)
	TSubclassOf<AEntitySpawner> EntitySpawnerClass;

	UPROPERTY()
	TObjectPtr<AEntitySpawner> GlobalEntitySpawner;

	float CullDistance = 1000.f;
	float WaypointArrivalThreshold = 10.f;
	
	float RefreshNavPathThreshold = 0.5f;
	float RefreshNavPathTime = 0.f;

	float GridCellSize = 80.0f;
	float EntityRadius = 40.0f;
};