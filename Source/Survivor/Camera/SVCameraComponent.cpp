// KJY

#include "SVCameraComponent.h"
#include "SVCameraMode.h"

USVCameraComponent::USVCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraModeCollection = nullptr;
	FieldOfViewOffset = 0.f;
}

USVCameraComponent* USVCameraComponent::FindCameraComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<USVCameraComponent>() : nullptr;
}

AActor* USVCameraComponent::GetTargetActor() const
{
	return GetOwner();
}

void USVCameraComponent::AddFieldOfViewOffset(float FovOffset)
{
	FieldOfViewOffset += FovOffset;
	FieldOfViewOffset = FMath::Clamp(FieldOfViewOffset, -30.f, 30.f);
}

void USVCameraComponent::SetCameraMode(const TSubclassOf<USVCameraMode> NewCameraMode)
{
	DesiredCameraMode = NewCameraMode;
}

void USVCameraComponent::ClearCameraMode()
{
	DesiredCameraMode = nullptr;
}

void USVCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeCollection)
	{
		CameraModeCollection = NewObject<USVCameraModeCollection>(this);
		check(CameraModeCollection);
	}
}

void USVCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeCollection);

	UpdateCameraModes();

	// 현재 Collection에 등록되어있는 CameraMode들을 집계해 가장 적절한 위치, FOV 등을 계산합니다.
	FSVCameraModeView CameraModeView;
	CameraModeCollection->EvaluateCollection(DeltaTime, CameraModeView);

	// DesiredView에 결과값을 채워넣습니다.
	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}
	
	CameraModeView.FieldOfView += FieldOfViewOffset;

	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);
	FieldOfView = CameraModeView.FieldOfView;

	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;

	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

void USVCameraComponent::UpdateCameraModes()
{
	check(CameraModeCollection);

	if (CameraModeCollection->IsCollectionActivate())
	{
		if (DesiredCameraMode)
		{
			CameraModeCollection->AddCameraMode(DesiredCameraMode);
		}
		else
		{
			CameraModeCollection->AddCameraMode(DefaultCameraMode);
		}
	}
}
