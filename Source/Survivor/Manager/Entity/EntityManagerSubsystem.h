// KJY

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EntityManagerSubsystem.generated.h"

class ASVEnemy;
class AEntitySpawner;

USTRUCT()
struct FEntityPool
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> InstancedStaticMeshComponent;

	TArray<FVector_NetQuantize> InstanceLocations;
	TArray<FRotator> InstanceRotations;
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

	void SpawnEntities(const uint8 MonsterID, const TArray<FVector>& NewLocations);

private:
	UPROPERTY()
	TMap<uint8, FEntityPool> EntityPools;

	UPROPERTY()
	TMap<uint8, TObjectPtr<ASVEnemy>> EnemyPools;

	UPROPERTY(Config)
	TSubclassOf<AEntitySpawner> EntitySpawnerClass;

	UPROPERTY()
	TObjectPtr<AEntitySpawner> GlobalEntitySpawner;
};
