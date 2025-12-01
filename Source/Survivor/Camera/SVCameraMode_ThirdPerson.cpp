// KJY

#include "SVCameraMode_ThirdPerson.h"

#include "SVCameraAssistInterface.h"
#include "SVPenetrationAvoidanceFeeler.h"
#include "Curves/CurveVector.h"
#include "GameFramework/CameraBlockingVolume.h"

namespace SVCameraMode_ThirdPerson_Statics
{
	static const FName NAME_IgnoreCameraCollision = TEXT("IgnoreCameraCollision");
}

USVCameraMode_ThirdPerson::USVCameraMode_ThirdPerson()
{
	TargetOffsetCurve = nullptr;
	
	PenetrationAvoidanceFeelers.Add(FSVPenetrationAvoidanceFeeler(FRotator(+00.0f, +00.0f, 0.0f), 14.f, 0));
	PenetrationAvoidanceFeelers.Add(FSVPenetrationAvoidanceFeeler(FRotator(+00.0f, +14.0f, 0.0f), 06.f, 5));
	PenetrationAvoidanceFeelers.Add(FSVPenetrationAvoidanceFeeler(FRotator(+00.0f, -14.0f, 0.0f), 06.f, 5));
	PenetrationAvoidanceFeelers.Add(FSVPenetrationAvoidanceFeeler(FRotator(+14.0f, +00.0f, 0.0f), 06.f, 4));
	PenetrationAvoidanceFeelers.Add(FSVPenetrationAvoidanceFeeler(FRotator(-14.0f, +00.0f, 0.0f), 06.f, 4));
}

void USVCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	Super::UpdateView(DeltaTime);

	// 현 Pitch 각도에 따라 Curve를 통해 카메라의 Location을 조정합니다.
	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(View.Rotation.Pitch);
		View.Location = View.Location + View.Rotation.RotateVector(TargetOffset);
	}

	UpdatePreventPenetration(DeltaTime);
}

void USVCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)
{
	if (!bPreventPenetration)
	{
		return;
	}

	AActor* TargetActor = GetTargetActor();
	const APawn* TargetPawn = Cast<APawn>(TargetActor);
	AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;

	/**
	 * 1. 실제 카메라는 (대부분의 경우)캐릭터로부터 꽤 멀리 떨어져있습니다.
	 * 2. 카메라로부터 뻗어져나온 직선위에 위치하며 TargetActor의 Location과 가장 가까운 Vector를 계산합니다.
	 * 3. 해당 Vector가 캐릭터의 Z축 길이를 벗어나지 않도록 Z축 값을 Clamp합니다.
	 * 4. 해당 Vector가 정확히 CapsuleComponent의 표면에 위치하도록 조정합니다.
	 * 5. 해당 Vector를 지정된 길이만큼 CapsuleComponent 바깥쪽으로 밀어냅니다.
	 */
	
	// View.Location으로부터 View.Rotation.Vector() 방향으로 뻗은 직선과
	// 해당 직선에서 SafeLocation과 가장 거리가 가까운 Vector를 구하는 과정입니다.
	FVector ClosestPointOnLineToCapsuleCenter;
	FVector SafeLocation = TargetActor->GetActorLocation();
	FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);

	// Z 위치가 캐릭터의 CapsuleComponent 최대/최소 Z 지점을 벗어나지 않도록 합니다.
	const float PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
	const float MaxHalfHeight = TargetActor->GetSimpleCollisionHalfHeight() - PushInDistance;
	SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

	// Ray 수행 시작 위치가 캐릭터 앞에 존재하는 경우를 방지하기 위해 SafeLocation을 카메라쪽으로 살짝 당겨줍니다.
	FVector CapsuleToCameraDirection = SafeLocation - View.Location;
	CapsuleToCameraDirection.Normalize();
	SafeLocation -= CapsuleToCameraDirection * 20.f;

	// 최종 SafeLocation을 사용해 Ray를 수행, 실제 카메라 위치를 계산합니다.
	const bool bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;
	PreventCameraPenetration(*TargetActor, SafeLocation, View.Location, View.Rotation, DeltaTime, AimLineToDesiredPosBlockedPercent, bSingleRayPenetrationCheck);

	// Hiding 임계점에 도달하면 PlayerController의 함수를 호출합니다.
	if (AimLineToDesiredPosBlockedPercent < ReportPenetrationPercent)
	{
		if (ISVCameraAssistInterface* TargetControllerAssist = Cast<ISVCameraAssistInterface>(TargetController))
		{
			TargetControllerAssist->OnCameraPenetratingTarget();
		}
	}
}

void USVCameraMode_ThirdPerson::PreventCameraPenetration(const AActor& ViewTarget, const FVector& SafeLocation, FVector& CameraLocation, FRotator& CameraRotation, float const& DeltaTime, float& DistBlockedPercent, bool bSingleRayOnly)
{
	float HardBlockedPercent = DistBlockedPercent;
	float SoftBlockedPercent = DistBlockedPercent;

	// Ray를 수행할 방향을 결정합니다.
	FVector BaseRay = CameraLocation - SafeLocation;
	FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
	FVector BaseRayLocalUp, BaseRayLocalForward, BaseRayLocalRight;

	// Ray를 수행할 방향에 대한 로컬 축 방향 벡터를 추출합니다.
	BaseRayMatrix.GetScaledAxes(BaseRayLocalForward, BaseRayLocalRight, BaseRayLocalUp);

	// 이번 프레임에서 Ray들이 얼마나 막혔는지를 측정할 값입니다.
	float DistBlockedPercentThisFrame = 1.f;

	// Ray 개수를 결정합니다.
	const int32 NumRaysToShoot = bSingleRayOnly ? FMath::Min(1, PenetrationAvoidanceFeelers.Num()) : PenetrationAvoidanceFeelers.Num();

	// 충돌 검사 방식을 설정합니다. 세부 내역은 아래 반복문에서 Feeler를 참조해 설정합니다.
	FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr);
	SphereParams.AddIgnoredActor(&ViewTarget);
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);
	
	UWorld* World = GetWorld();

	for (int32 RayIndex = 0; RayIndex < NumRaysToShoot; ++RayIndex)
	{
		FSVPenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIndex];
		// 이번 Feeler가 트레이스 가능 상태인지(쿨다운) 체크합니다.
		if (Feeler.FramesUntilNextTrace <= 0)
		{
			FVector RayTarget;
			{
				// 이번 Feeler의 방향을 결정합니다.
				FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
				RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
				RayTarget = SafeLocation + RotatedRay;
			}

			// 충돌 검사 방식을 재설정합니다.
			SphereShape.Sphere.Radius = Feeler.Extent;
			ECollisionChannel TraceChannel = ECC_Camera;

			// Ray 캐스트를 실행합니다.
			FHitResult Hit;
			const bool bHit = World->SweepSingleByChannel(Hit, SafeLocation, RayTarget, FQuat::Identity, TraceChannel, SphereShape, SphereParams);

			// Ray를 수행한 Feeler이므로, 일단 쿨다운을 적용합니다.
			Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

			const AActor* HitActor = Hit.GetActor();

			if (bHit && HitActor)
			{
				bool bIgnoreHit = false;

				// 검출된 Actor가 무시 대상인지 확인합니다.
				if (HitActor->ActorHasTag(SVCameraMode_ThirdPerson_Statics::NAME_IgnoreCameraCollision))
				{
					bIgnoreHit = true;
					SphereParams.AddIgnoredActor(HitActor); 
				}

				// 검출된 Actor가 CameraBlockingVolume이라면 해당 Volume을 무시할지 판단을 시작합니다.
				if (!bIgnoreHit && HitActor->IsA<ACameraBlockingVolume>())
				{
					// 카메라의 정면 방향을 정규화합니다.
					const FVector CameraForwardXY = CameraRotation.Vector().GetSafeNormal2D();
					// TargetActor의 월드 위치를 가져옵니다.
					const FVector ViewTargetLocation = ViewTarget.GetActorLocation();
					// TargetActor에서 Hit Location까지의 방향 벡터를 정규화합니다.
					const FVector HitOffset = Hit.Location - ViewTargetLocation;
					const FVector HitDirectionXY = HitOffset.GetSafeNormal2D();
					// 두 벡터의 내적을 구합니다.
					const float DotHitDirection = FVector::DotProduct(CameraForwardXY, HitDirectionXY);
					// 계산 결과 충돌 지점이 카메라 기준으로 캐릭터보다 더 멀리 있다면 이번 충돌을 무시합니다.
					if (DotHitDirection > 0.f)
					{
						bIgnoreHit = true;
						SphereParams.AddIgnoredActor(HitActor);
					}
				}

				if (!bIgnoreHit)
				{
					// Ray 길이와 카메라가 있을 위치를 비교해 얼마나 막혔는지 0.0 ~ 1.0으로 계산합니다.
					float NewBlockPercent = ((Hit.Location - SafeLocation).Size() - CollisionPushOutDistance) / (RayTarget - SafeLocation).Size();
					// 반복문 내에서 계산 중이므로, 이전 계산과 이번 계산 중 더 작은 수치를 기록합니다.
					DistBlockedPercentThisFrame = FMath::Min(NewBlockPercent, DistBlockedPercentThisFrame);

					// 충돌이 있다고 판단된 Feeler이므로, 다음 프레임에서도 Ray를 수행해야 합니다.
					Feeler.FramesUntilNextTrace = 0;
				}
			}

			if (RayIndex == 0)
			{
				// 주변 Ray가 아닌, 카메라 방향 그대로 나오는 Ray입니다.
				// 따라서 Snap이 발생하더라도 즉시 카메라를 이동시켜야 합니다.
				HardBlockedPercent = DistBlockedPercentThisFrame;
			}
			else
			{
				// 주변 Ray로 계산된 경우 SoftBlockedPercent에 기록합니다.
				SoftBlockedPercent = DistBlockedPercentThisFrame;
			}
		}
		else
		{
			// Ray를 수행하지 않은 Feeler이므로 쿨다운용 값을 1 감소시킵니다.
			--Feeler.FramesUntilNextTrace;
		}
	}

	// DistBlockedPercent: 기존 충돌 상태
	// DistBlockedPercentThisFrame: 이번 충돌 상태
	// HardBlockedPercent: 이번 0번째 인덱스 Feeler 충돌 상태
	// SoftBlockedPercent: 주변 Ray 충돌 상태 중 가장 작은 수치(가장 카메라를 캐릭터쪽으로 많이 밀어야 하는 수치)
	
	if (DistBlockedPercent < DistBlockedPercentThisFrame)
	{
		// 기존 충돌이 없어져 카메라가 캐릭터로부터 멀어지는 경우 들어오는 분기입니다.
		if (PenetrationBlendOutTime > DeltaTime)
		{
			// 보간할 시간이 남은 경우 들어오는 분기입니다.
			DistBlockedPercent = DistBlockedPercent + DeltaTime / PenetrationBlendOutTime * (DistBlockedPercentThisFrame - DistBlockedPercent);
		}
		else
		{
			// 보간할 시간이 없는 경우 들어오는 분기입니다.
			// 프레임 드랍 등으로 인한 시간 초과 상황을 고려한 예외처리입니다.
			DistBlockedPercent = DistBlockedPercentThisFrame;
		}
	}
	else
	{
		// 새로운 충돌로 인해 카메라가 캐릭터쪽으로 가까워지거나, 기존 상태를 유지해야 하는 경우 들어오는 분기입니다.
		if (DistBlockedPercent > HardBlockedPercent)
		{
			// 0번째 인덱스 Feeler 충돌 상태가 기존 충돌 상태보다 캐릭터쪽에 가까운 경우 즉시 Snap합니다.
			DistBlockedPercent = HardBlockedPercent;
		}
		else if (DistBlockedPercent > SoftBlockedPercent)
		{
			// Soft이므로 보간합니다.
			if (PenetrationBlendInTime > DeltaTime)
			{
				DistBlockedPercent = DistBlockedPercent - DeltaTime / PenetrationBlendInTime * (DistBlockedPercent - SoftBlockedPercent);
			}
			else
			{
				DistBlockedPercent = SoftBlockedPercent;
			}
		}
	}

	// 이제 DistBlockedPercent는 이번 충돌 상태를 나타냅니다.

	DistBlockedPercent = FMath::Clamp(DistBlockedPercent, 0.f, 1.f);
	if (DistBlockedPercent < (1.f - ZERO_ANIMWEIGHT_THRESH))
	{
		// 계산 결과 카메라를 캐릭터쪽으로 밀어야 하는 경우, DistBlockedPercent를 사용해 최종 CameraLocation을 계산합니다.
		CameraLocation = SafeLocation + (CameraLocation - SafeLocation) * DistBlockedPercent;
	}
}
