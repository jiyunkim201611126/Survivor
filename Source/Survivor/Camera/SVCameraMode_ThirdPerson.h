// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVCameraMode.h"
#include "SVCameraMode_ThirdPerson.generated.h"

struct FSVPenetrationAvoidanceFeeler;
class UCurveVector;
/**
 * 3인칭 시점을 표현하는 클래스입니다.
 * 캐릭터와 카메라 사이에 충돌체가 있을 경우 카메라가 해당 충돌체 앞으로 이동하는 로직을 포함합니다.
 */
UCLASS(Abstract, Blueprintable)
class SURVIVOR_API USVCameraMode_ThirdPerson : public USVCameraMode
{
	GENERATED_BODY()

public:
	USVCameraMode_ThirdPerson();

protected:
	virtual void UpdateView(float DeltaTime) override;

	// 현재 카메라 위치에서 캐릭터와 가장 가까울 수 있는 지점을 계산 및 그에 따른 실제 카메라 위치를 계산하는 함수입니다.
	void UpdatePreventPenetration(float DeltaTime);
	void PreventCameraPenetration(const AActor& ViewTarget, const FVector& SafeLocation, FVector& CameraLocation, FRotator& CameraRotation, float const& DeltaTime, float& DistBlockedPercent, bool bSingleRayOnly);

protected:
	// Pitch 조작에 따라 카메라의 위치를 변경하는 데에 사용되는 변수들입니다. 
	// Curve 에셋을 사용하는 것으로, 다른 CameraMode와 공유할 수 있는 Curve입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;

public:
	// 충돌이 발생한 경우 카메라가 계산 결과값까지 가는 데에 걸리는 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PenetrationBlendInTime = 0.1;

	// 기존 충돌이 없어진 경우 새로운 계산 결과값까지 가는 데에 걸리는 시간입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PenetrationBlendOutTime = 0.15f;

	// true일 경우, 카메라가 충돌을 감지해 캐릭터와 카메라 사이에 물체가 있을 경우 그 물체 앞으로 이동합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bPreventPenetration = true;

	// 충돌을 예상해서 카메라를 움직일지에 대한 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bDoPredictiveAvoidance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionPushOutDistance = 2.f;

	// 캐릭터와 가장 가까운 카메라 위치를 0.0, 충돌체가 없을 경우의 카메라 위치를 1.0으로 표현합니다. 
	// 이 변수보다 작은 상태일 경우 캐릭터를 포함한 Attach된 컴포넌트와 액터까지 모두 렌더링을 끕니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float ReportPenetrationPercent = 0.f;

	/**
	 * 카메라 주변의 추가적인 라인 트레이스를 수행, 충돌을 미리 예측해 카메라의 텔레포트를 방지합니다.
	 * Index 0  : 항시 수행되는 기본 라인 트레이스입니다.
	 * Index 1+ : bDoPredictiveAvoidance가 true일 경우에만 수행되는 추가 라인트레이스입니다.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<FSVPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	// 현재 카메라 위치 보정 상태를 나타냅니다. (캐릭터 위치 0.0 ~ 충돌이 없는 경우 카메라 위치 1.0)
	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPercent = 1.f;

private:
	// 플레이어의 캐릭터가 아닌, 현재 Hide된 다른 Pawn입니다.
	UPROPERTY()
	TArray<TObjectPtr<AActor>> HidedOtherPawns;
};
