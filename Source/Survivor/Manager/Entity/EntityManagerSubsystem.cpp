// KJY

#include "EntityManagerSubsystem.h"

#include "EntitySpawner.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UEntityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!GlobalEntitySpawner)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GlobalEntitySpawner = GetWorld()->SpawnActor<AEntitySpawner>(EntitySpawnerClass, SpawnParameters);
	}
}

void UEntityManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UPawnManagerSubsystem* PawnManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
	if (!PawnManagerSubsystem || PawnManagerSubsystem->GetAllPlayerPawns().Num() == 0 || !GlobalEntitySpawner)
	{
		return;
	}

	const AActor* TargetPawn = PawnManagerSubsystem->GetAllPlayerPawns()[0];
	const FVector PlayerPos = TargetPawn->GetActorLocation();

	// 캐릭터와 거리가 아래 값 이하인 Entity만 InstancedMesh로 렌더링합니다. 
	constexpr float CullDistance = 1000.f;
	constexpr float CullDistanceSquared = CullDistance * CullDistance;

	for (auto& MonsterPool : EntityPools)
	{
		FEntityPool& Pool = MonsterPool.Value;
		if (!Pool.InstancedStaticMeshComponent)
		{
			continue;
		}

		// 다음에 렌더링될 Entity들의 위치 정보입니다.
		TArray<FVector_NetQuantize> NextLocations;
		TArray<FTransform> NextTransforms;
		NextLocations.Reserve(Pool.InstanceLocations.Num());
		NextTransforms.Reserve(Pool.InstanceLocations.Num());

		const float MonsterSpeed = GlobalEntitySpawner->GetMonsterSpeed(MonsterPool.Key);

		for (const FVector_NetQuantize& CurrentLocation : Pool.InstanceLocations)
		{
			// 모든 Entity들의 위치를 플레이어쪽으로 다가가게 한 후 거리에 따라 렌더링할지 결정합니다.
			// TODO: NavMesh와 상호작용하도록 만들어야 합니다.
			const FVector NewLocation = FMath::VInterpTo(CurrentLocation, PlayerPos, DeltaTime, MonsterSpeed / 1000.f);

			if (FVector::DistSquared(PlayerPos, NewLocation) > CullDistanceSquared)
			{
				// 일정 거리보다 멀어 InstancedMesh로 렌더링해야 하는 경우 들어오는 분기입니다.
				NextLocations.Add(NewLocation);

				// InstancedMesh를 렌더링할 위치를 업데이트합니다.
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(NewLocation, PlayerPos);
				LookAtRotation.Pitch = 0.f;
				LookAtRotation.Roll = 0.f;
				NextTransforms.Add(FTransform(LookAtRotation, NewLocation, FVector::OneVector));
			}
			else
			{
				// TODO: GASActor로 교체합니다.
				// 한 번 GASActor로 교체되고 난 후에는 멀어져도 다시 InstancedMesh로 변경되지 않습니다.
			}
		}

		// 다음 프레임을 위해 위치 목록을 업데이트합니다.
		Pool.InstanceLocations = NextLocations;

		// 현재 인스턴스 개수와 필요한 개수를 비교하여 차이만큼 추가하거나 제거합니다.
		const int32 CurrentInstanceCount = Pool.InstancedStaticMeshComponent->GetInstanceCount();
		const int32 NextInstanceCount = NextTransforms.Num();
		const int32 Delta = NextInstanceCount - CurrentInstanceCount;

		if (Delta > 0)
		{
			TArray<FTransform> TempTransforms;
			TempTransforms.AddDefaulted(Delta);
			Pool.InstancedStaticMeshComponent->AddInstances(TempTransforms, false);
		}
		else if (Delta < 0)
		{
			TArray<int32> IndicesToRemove;
			IndicesToRemove.Reserve(-Delta);
			for (int i = 0; i < -Delta; ++i)
			{
				IndicesToRemove.Add(CurrentInstanceCount - 1 - i);
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

void UEntityManagerSubsystem::SpawnEntities(const uint8 MonsterID, const TArray<FVector>& NewLocations)
{
	if (NewLocations.IsEmpty())
	{
		return;
	}

	// MonsterID에 해당하는 Pool이 없으면 추가하고 가져옵니다.
	FEntityPool& Pool = EntityPools.FindOrAdd(MonsterID);

	if (!Pool.InstancedStaticMeshComponent)
	{
		Pool.InstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(GlobalEntitySpawner);

		UStaticMesh* MonsterMesh = GlobalEntitySpawner->GetMonsterMesh(MonsterID);
		Pool.InstancedStaticMeshComponent->SetStaticMesh(MonsterMesh);

		Pool.InstancedStaticMeshComponent->RegisterComponent();
	}

	// 기존 위치 목록에 새로운 위치를 추가합니다.
	Pool.InstanceLocations.Append(NewLocations);

	// 새로 추가된 위치에 대한 트랜스폼을 생성합니다.
	TArray<FTransform> NewTransforms;
	NewTransforms.Reserve(NewLocations.Num());
	for (const FVector& Location : NewLocations)
	{
		NewTransforms.Emplace(Location);
	}

	// 새로운 인스턴스를 배치 추가합니다.
	Pool.InstancedStaticMeshComponent->AddInstances(NewTransforms, false);
}

TStatId UEntityManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UEntityManagerSubsystem, STATGROUP_Game);
}
