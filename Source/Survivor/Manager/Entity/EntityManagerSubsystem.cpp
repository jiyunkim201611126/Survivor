// KJY

#include "EntityManagerSubsystem.h"

#include "EntitySpawner.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UEntityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitEntitySpawner();
}

void UEntityManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UPawnManagerSubsystem* PawnManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
	if (!PawnManagerSubsystem || !GlobalEntitySpawner)
	{
		return;
	}

	if (const APawn* LocalPawn = PawnManagerSubsystem->GetLocalPlayerPawn())
	{
		// 로컬 캐릭터와 거리가 아래 값 이하인 Entity만 InstancedMesh로 렌더링합니다. 
		constexpr float CullDistance = 1000.f;
		constexpr float CullDistanceSquared = CullDistance * CullDistance;
		
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

			for (FEntityInstanceData& InstanceData : Pool.Instances)
			{
				if (!InstanceData.Target)
				{
					// Target이 모종의 이유로 더이상 유효하지 않을 경우, 다른 랜덤 Target을 결정합니다.
					InstanceData.Target = PawnManagerSubsystem->GetRandomPlayerPawn();
				}
			
				const APawn* TargetPawn = InstanceData.Target;

				if (!TargetPawn)
				{
					// 유효한 플레이어가 한 명도 없으면 데이터를 그대로 유지합니다.
					NextInstances.Emplace(InstanceData);
					continue;
				}
			
				const FVector TargetPos = TargetPawn->GetActorLocation();
				const FVector CurrentLocation = InstanceData.Transform.GetLocation();

				// TODO: NavMesh와 상호작용하는 MoveTo 함수를 만들어야 합니다.
				const FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetPos, DeltaTime, MonsterSpeed / 1000.f);
			
				if (FVector::DistSquared(LocalPawnLocation, NewLocation) > CullDistanceSquared)
				{
					// 일정 거리보다 멀어 InstancedMesh로 렌더링해야 하는 경우, NextInstances에 추가합니다.
					InstanceData.Transform.SetLocation(NewLocation);

					FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(NewLocation, TargetPos);
					LookAtRotation.Pitch = 0.f;
					LookAtRotation.Roll = 0.f;
					InstanceData.Transform.SetRotation(LookAtRotation.Quaternion());

					// 업데이트된 인스턴스 데이터를 다음 프레임 목록에 추가합니다.
					NextInstances.Add(InstanceData);
				}
				else
				{
					// TODO: GASActor로 교체합니다.
					// 이 경우, 해당 몬스터 정보는 NextInstances에 추가되지 않으므로 자연스럽게 InstancedMesh 목록에서 제거됩니다.
				}
			}

			// 다음 프레임을 위해 인스턴스 목록을 통째로 업데이트합니다.
			Pool.Instances = NextInstances;

			// ISM 렌더링을 위해 트랜스폼 목록을 준비합니다.
			TArray<FTransform> NextTransforms;
			NextTransforms.Reserve(Pool.Instances.Num());
			for (const FEntityInstanceData& InstanceData : Pool.Instances)
			{
				NextTransforms.Add(InstanceData.Transform);
			}
		
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
				for (int i = 0; i < -Delta; ++i)
				{
					IndicesToRemove.Emplace(CurrentInstanceCount - 1 - i);
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
		
		NewTransforms.Add(NewData.Transform);
	}
	
	Pool.InstancedStaticMeshComponent->AddInstances(NewTransforms, false);
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
