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
	TSubclassOf<ASVEnemy> GASActorClass;

	UPROPERTY(EditDefaultsOnly)
	float MovementSpeed = 300.f;
};

UCLASS(BlueprintType)
class SURVIVOR_API UMonsterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UStaticMesh* GetMonsterMesh(const uint8 MonsterID);
	TSubclassOf<ASVEnemy> GetGASActorClass(const uint8 MonsterID);
	float GetMonsterSpeed(const uint8 MonsterID);

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<uint8, FMonsterData> MonsterDatas;
};

UCLASS()
class SURVIVOR_API AEntitySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEntitySpawner();

	UMonsterDataAsset* GetMonsterDataAsset();

	UStaticMesh* GetMonsterMesh(const uint8 MonsterID) const;
	TSubclassOf<ASVEnemy> GetGASActorClass(const uint8 MonsterID) const;
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
