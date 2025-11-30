// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVPenetrationAvoidanceFeeler.generated.h"

/**
 * 카메라 주변에 추가적인 라인트레이스 범위 및 방향으로, 충돌을 예측해 카메라가 순간적으로 텔레포트하는 것을 방지하는 역할의 구조체입니다.
 */
USTRUCT()
struct FSVPenetrationAvoidanceFeeler
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "PenetrationAvoidanceFeeler")
	FRotator AdjustmentRot;

	UPROPERTY(EditAnywhere, Category = "PenetrationAvoidanceFeeler")
	float Extent;

	UPROPERTY(EditAnywhere, Category = "PenetrationAvoidanceFeeler")
	int32 TraceInterval;

	UPROPERTY(Transient)
	int32 FramesUntilNextTrace;

	FSVPenetrationAvoidanceFeeler()
		: AdjustmentRot(ForceInit)
		, Extent(0)
		, TraceInterval(0)
		, FramesUntilNextTrace(0)
	{
	}
	
	FSVPenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
									const float& InExtent, 
									const int32& InTraceInterval = 0, 
									const int32& InFramesUntilNextTrace = 0)
		: AdjustmentRot(InAdjustmentRot)
		, Extent(InExtent)
		, TraceInterval(InTraceInterval)
		, FramesUntilNextTrace(InFramesUntilNextTrace)
	{
	}
};