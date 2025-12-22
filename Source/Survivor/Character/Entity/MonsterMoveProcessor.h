// KJY

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MonsterMoveProcessor.generated.h"

/**
 * 몬스터 Entity들이 실제로 움직일 수 있도록 제어하는 클래스입니다.
 */
UCLASS()
class SURVIVOR_API UMonsterMoveProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UMonsterMoveProcessor();

protected:
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	// 몬스터를 찾아낼 쿼리입니다.
	FMassEntityQuery EntityQuery;
};
