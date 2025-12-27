// KJY

#include "EntityManagerSubsystem.h"

#include "EntitySpawner.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"
#include "Components/InstancedStaticMeshComponent.h"

void UEntityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitEntitySpawner();
}

void UEntityManagerSubsystem::InitEntitySpawner()
{
	if (!GlobalEntitySpawner)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GlobalEntitySpawner = GetWorld()->SpawnActor<AEntitySpawner>(EntitySpawnerClass, SpawnParameters);
	}
}

void UEntityManagerSubsystem::SpawnEntities(const uint8 MonsterID, const TArray<FVector>& NewLocations, APawn* TargetPawn)
{
	if (NewLocations.IsEmpty() || !GlobalEntitySpawner)
	{
		return;
	}

	// MonsterID에 해당하는 Pool이 없으면 추가하고 가져옵니다.
	FEntityPool& Pool = EntityPools.FindOrAdd(MonsterID);
	if (!Pool.InstancedStaticMeshComponent)
	{
		UStaticMesh* MonsterMesh = GlobalEntitySpawner->GetMonsterMesh(MonsterID);
		if (!MonsterMesh)
		{
			return;
		}
		
		Pool.InstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(GlobalEntitySpawner);
		Pool.InstancedStaticMeshComponent->SetStaticMesh(MonsterMesh);
		Pool.InstancedStaticMeshComponent->SetEvaluateWorldPositionOffset(true);
		Pool.InstancedStaticMeshComponent->RegisterComponent();
	}

	// 새로 추가된 위치에 대한 인스턴스 데이터와 트랜스폼을 생성합니다.
	TArray<FTransform> NewTransforms;
	NewTransforms.Reserve(NewLocations.Num());
	
	for (const FVector& Location : NewLocations)
	{
		FEntityInstanceData& NewData = Pool.Instances.Emplace_GetRef();
		NewData.Transform = FTransform(Location);
		NewData.Target = TargetPawn;
		NewData.PathPoints.Empty();
		NewData.CurrentPathIndex = 0;
		
		NewTransforms.Add(NewData.Transform);
	}
	
	Pool.InstancedStaticMeshComponent->AddInstances(NewTransforms, false);
}

void UEntityManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UPawnManagerSubsystem* PawnManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
	const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!PawnManagerSubsystem || !GlobalEntitySpawner || !NavSystem)
	{
		return;
	}

	if (const APawn* LocalPawn = PawnManagerSubsystem->GetLocalPlayerPawn())
	{
		// 로컬 캐릭터와 거리가 아래 값 이하인 Entity만 InstancedMesh로 렌더링합니다. 
		constexpr float CullDistance = 1000.f;
		constexpr float CullDistanceSquared = CullDistance * CullDistance;

		// 경로에 도착했는지 판별하는 기준 거리입니다.
		constexpr float WaypointArrivalThreshold = 10.f;
		constexpr float WaypointArrivalThresholdSquared = WaypointArrivalThreshold * WaypointArrivalThreshold;
		
		const FVector LocalPawnLocation = LocalPawn->GetActorLocation();
	
		for (auto& PoolPair : EntityPools)
		{
			FEntityPool& Pool = PoolPair.Value;
			if (!Pool.InstancedStaticMeshComponent)
			{
				continue;
			}

			// 다음 프레임에 살아남을 인스턴스 정보입니다.
			TArray<FEntityInstanceData> NextInstances;
			NextInstances.Reserve(Pool.Instances.Num());

			const uint8 MonsterID = PoolPair.Key;
			const float MonsterSpeed = GlobalEntitySpawner->GetMonsterSpeed(MonsterID);

			TArray<FTransform> NextTransforms;
			NextTransforms.Reserve(Pool.Instances.Num());
			
			for (FEntityInstanceData& InstanceData : Pool.Instances)
			{
				if (!InstanceData.Target)
				{
					// Target이 모종의 이유로 더이상 유효하지 않을 경우, 다른 랜덤 Target을 결정합니다.
					InstanceData.Target = PawnManagerSubsystem->GetRandomPlayerPawn();
				}
			
				APawn* TargetPawn = InstanceData.Target;
				if (!TargetPawn)
				{
					// 유효한 플레이어가 한 명도 없으면 데이터를 그대로 유지합니다.
					NextInstances.Emplace(InstanceData);
					continue;
				}

				// NavMesh 기반으로 TargetPawn을 향해 움직입니다.
				MoveToTargetWithNavMesh(InstanceData, NavSystem, TargetPawn, DeltaTime, MonsterSpeed, WaypointArrivalThresholdSquared);
			
				if (FVector::DistSquared(LocalPawnLocation, InstanceData.Transform.GetLocation()) > CullDistanceSquared)
				{
					// 일정 거리보다 멀어 InstancedMesh로 렌더링해야 하는 경우, NextInstances에 추가합니다.
					NextInstances.Add(InstanceData);
					
					// ISM 렌더링을 위해 트랜스폼 목록을 준비합니다.
					NextTransforms.Add(InstanceData.Transform);
				}
				else
				{
					// LocalPawn과 가까워졌으므로, GAS 관련 Component를 가진 Actor로 교체합니다.
				}
			}

			// 다음 프레임을 위해 인스턴스 목록을 통째로 업데이트합니다.
			Pool.Instances = NextInstances;
		
			// 현재 인스턴스 개수와 필요한 개수를 비교하여 차이만큼 추가하거나 제거합니다.
			const int32 CurrentInstanceCount = Pool.InstancedStaticMeshComponent->GetInstanceCount();
			const int32 NextInstanceCount = NextTransforms.Num();
			const int32 Delta = NextInstanceCount - CurrentInstanceCount;

			if (Delta > 0)
			{
				// 인스턴스를 추가해야 하는 경우 들어오는 분기입니다.
				TArray<FTransform> TempTransforms;
				TempTransforms.AddDefaulted(Delta);
				Pool.InstancedStaticMeshComponent->AddInstances(TempTransforms, false);
			}
			else if (Delta < 0)
			{
				// 인스턴스를 제거해야 하는 경우 들어오는 분기입니다.
				// 인덱스 기반 매핑이 꼬이지 않도록 하기 위해 뒤쪽에서부터 제거합니다.
				TArray<int32> IndicesToRemove;
				IndicesToRemove.Reserve(-Delta);
				for (int32 Index = 0; Index < -Delta; ++Index)
				{
					IndicesToRemove.Emplace(CurrentInstanceCount - 1 - Index);
				}
				Pool.InstancedStaticMeshComponent->RemoveInstances(IndicesToRemove);
			}
		
			// 모든 인스턴스의 트랜스폼을 최종적으로 한 번에 업데이트합니다.
			if (NextInstanceCount > 0)
			{
				Pool.InstancedStaticMeshComponent->BatchUpdateInstancesTransforms(0, NextTransforms, true, true);
			}
		}
	}
}

void UEntityManagerSubsystem::MoveToTargetWithNavMesh(FEntityInstanceData& InstanceData, const UNavigationSystemV1* NavSystem, APawn* TargetPawn, const float DeltaTime, const float MonsterSpeed, const float WaypointArrivalThresholdSquared)
{
	// 경로가 없거나 경로를 갱신한지 0.5초가 지나면 경로를 재생성합니다.
	if (InstanceData.PathPoints.Num() == 0 || RefreshNavPathThreshold <= RefreshNavPathTime)
	{
		const UNavigationPath* NavPath = NavSystem->FindPathToActorSynchronously(this, InstanceData.Transform.GetLocation(), TargetPawn);
		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			InstanceData.PathPoints = NavPath->PathPoints;
			InstanceData.CurrentPathIndex = 1;

			// Material에서 사용할 수 있도록 목적지를 향하는 Rotation을 Transform에 넣어줍니다.
			// InstancedMesh가 이 값으로 인해 회전하지만, Material의 WorldPositionOffset에 의해 덮어씌워져 실제로는 다른 방향을 바라보도록 렌더링됩니다.
			// ISM의 SetNumCustomDataFloats 함수를 사용하는 방법도 있었으나, Transform의 Rotation 공간이 낭비되고 있어 이 방법을 채택했습니다. 
			const FVector& InstanceLocation = InstanceData.Transform.GetLocation();
			const FVector& TargetLocation = NavPath->PathPoints[1];
			const FVector InstanceDirection = TargetLocation - InstanceLocation;
			InstanceData.Transform.SetRotation(InstanceDirection.Rotation().Quaternion());
		}

		RefreshNavPathTime = 0.f;
	}
	else
	{
		RefreshNavPathTime += DeltaTime;
	}

	// 경로가 있으면 해당 경로로 이동합니다.
	if (InstanceData.PathPoints.Num() > 0 && InstanceData.PathPoints.IsValidIndex(InstanceData.CurrentPathIndex))
	{
		const FVector CurrentLocation = InstanceData.Transform.GetLocation();
		const FVector Waypoint = InstanceData.PathPoints[InstanceData.CurrentPathIndex];

		// 다음 목적지까지의 벡터와 거리 제곱을 구합니다.
		const FVector VectorToWaypoint = Waypoint - CurrentLocation;
		const float ToWaypointDistSquared = VectorToWaypoint.SizeSquared();

		// 이번 프레임에 나아가야 하는 거리입니다.
		const float ThisFrameStepLength = MonsterSpeed * DeltaTime;
		const float ThisFrameStepSquared = FMath::Square(ThisFrameStepLength);
		
		FVector NewLocation;
		if (ToWaypointDistSquared < ThisFrameStepSquared)
		{
			// 이번 프레임에 목적지를 지나치는 경우 목적지 위치를 그대로 사용합니다.
			NewLocation = Waypoint;
		}
		else
		{
			// 이번 프레임에 목적지에 도착하지 못 한 경우 일정한 속도로 계속 나아가도록 위치를 갱신합니다.
			NewLocation = CurrentLocation + VectorToWaypoint.GetSafeNormal() * ThisFrameStepLength;
		}
		
		InstanceData.Transform.SetLocation(NewLocation);
		
		// 목적지에 도착했는지 검사합니다.
		if (FVector::DistSquared(NewLocation, Waypoint) < WaypointArrivalThresholdSquared)
		{
			// 일단 다음 경로를 향해 나아갈 수 있도록 값을 더해줍니다.
			// 단, 0.5초마다 경로를 재탐색하기 때문에 단순히 Entity가 멈추지 않게 하는 의도일 뿐입니다.
			InstanceData.CurrentPathIndex++;
			// 경로의 끝에 도달했으면 경로를 비워서 다음 틱에 새로 탐색하도록 합니다.
			if (!InstanceData.PathPoints.IsValidIndex(InstanceData.CurrentPathIndex))
			{
				InstanceData.PathPoints.Empty();
			}
		}
	}
	else
	{
		// 경로가 없거나, 경로 인덱스가 잘못된 경우, 다음 틱에 경로를 다시 탐색하도록 경로를 비웁니다.
		InstanceData.PathPoints.Empty();
	}
}

APawn* UEntityManagerSubsystem::GetEntityTarget(const FEntityPool& Pool, const int32 InstanceIndex) const
{
	if (Pool.Instances.IsValidIndex(InstanceIndex))
	{
		return Pool.Instances[InstanceIndex].Target;
	}
	return nullptr;
}

TStatId UEntityManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UEntityManagerSubsystem, STATGROUP_Game);
}
