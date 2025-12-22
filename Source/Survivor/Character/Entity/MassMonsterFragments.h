#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassMonsterFragments.generated.h"

// 몬스터의 Transform을 담당하는 데이터입니다.
USTRUCT()
struct FMonsterTransformFragment : public FMassFragment
{
	GENERATED_BODY()

	FTransform Transform;
};
