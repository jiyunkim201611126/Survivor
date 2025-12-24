// KJY

#include "EntitySpawner.h"

#include "EntityManagerSubsystem.h"
#include "Algo/RandomShuffle.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UStaticMesh* UMonsterDataAsset::GetMonsterMesh(const uint8 MonsterID)
{
	if (const FMonsterData* MonsterData = MonsterDatas.Find(MonsterID))
	{
		return MonsterData->EntityMesh;
	}

	return nullptr;
}

TSubclassOf<ASVEnemy> UMonsterDataAsset::GetGASActorClass(const uint8 MonsterID)
{
	if (const FMonsterData* MonsterData = MonsterDatas.Find(MonsterID))
	{
		return MonsterData->GASActorClass;
	}

	return nullptr;
}

float UMonsterDataAsset::GetMonsterSpeed(const uint8 MonsterID)
{
	if (const FMonsterData* MonsterData = MonsterDatas.Find(MonsterID))
	{
		return MonsterData->MovementSpeed;
	}

	return 0.f;
}

AEntitySpawner::AEntitySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

UMonsterDataAsset* AEntitySpawner::GetMonsterDataAsset()
{
	return MonsterDataAsset;
}

UStaticMesh* AEntitySpawner::GetMonsterMesh(const uint8 MonsterID) const
{
	return MonsterDataAsset->GetMonsterMesh(MonsterID);
}

TSubclassOf<ASVEnemy> AEntitySpawner::GetGASActorClass(const uint8 MonsterID) const
{
	return MonsterDataAsset->GetGASActorClass(MonsterID);
}

float AEntitySpawner::GetMonsterSpeed(const uint8 MonsterID) const
{
	return MonsterDataAsset->GetMonsterSpeed(MonsterID);
}

void AEntitySpawner::SpawnEntities()
{
	FEnvQueryRequest QueryRequest(EntitySpawnLocationQuery, GetWorld()->GetFirstPlayerController()->GetPawn());
	QueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &ThisClass::OnQueryFinished);
}

void AEntitySpawner::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result) const
{
	// EQS로 받아온 Result에서 Location들을 가져옵니다.
	TArray<FVector> Locations;
	Result->GetAllAsLocations(Locations);

	// 이 중 랜덤하게 몇 개의 위치만 사용합니다.
	// TODO: 생존 시간, 웨이브 수 등으로 이 개수를 결정하는 알고리즘이 필요합니다.
	Algo::RandomShuffle(Locations);
	Locations.SetNum(100);

	if (UEntityManagerSubsystem* EntityManagerSubsystem = GetWorld()->GetSubsystem<UEntityManagerSubsystem>())
	{
		EntityManagerSubsystem->SpawnEntities(0, Locations);
	}
}
