// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EntitySpawner.generated.h"

struct FEnvQueryResult;
class UEnvQuery;
class ASVEnemy;

USTRUCT(BlueprintType)
struct FMonsterData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> EntityMesh;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> GASActorClass;

	UPROPERTY(EditDefaultsOnly)
	float MovementSpeed = 300.f;
};

UCLASS(BlueprintType)
class SURVIVOR_API UMonsterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UStaticMesh* GetMonsterMesh(const uint8 MonsterID);
	TSubclassOf<AActor> GetGASActorClass(const uint8 MonsterID);
	float GetMonsterSpeed(const uint8 MonsterID);

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<uint8, FMonsterData> MonsterDatas;
};

/**
 * EntityManagerSubsystem과 밀접하게 관련 있는 클래스입니다.
 * Subsystem은 Replicate되지 않는다는 점과, EditDefaultOnly 등의 UPROPERTY 매크로 사용 편의성을 위해 선언되었습니다.
 * 또한 Subsystem은 Entity의 총괄, 해당 클래스는 Spawn될 위치 계산과 타이밍 결정 등의 역할을 담당하는 관심사의 분리가 되어있습니다.
 */
UCLASS()
class SURVIVOR_API AEntitySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEntitySpawner();

	UMonsterDataAsset* GetMonsterDataAsset();

	UStaticMesh* GetMonsterMesh(const uint8 MonsterID) const;
	TSubclassOf<AActor> GetGASActorClass(const uint8 MonsterID) const;
	float GetMonsterSpeed(const uint8 MonsterID) const;
	
	UFUNCTION(BlueprintCallable)
	void SpawnEntities();

	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Monster|Data")
	TObjectPtr<UMonsterDataAsset> MonsterDataAsset;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UEnvQuery> EntitySpawnLocationQuery;
};
