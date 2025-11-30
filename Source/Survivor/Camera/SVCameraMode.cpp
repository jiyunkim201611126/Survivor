// KJY

#include "SVCameraMode.h"

#include "SVCameraComponent.h"

//////////////////////////////////////////////////////////////////////
// FSVCameraModeView
FSVCameraModeView::FSVCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(80.f)
{
}

void FSVCameraModeView::Blend(const FSVCameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.f)
	{
		return;
	}
	if (OtherWeight >= 1.f)
	{
		*this = Other;
		return;
	}

	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

//////////////////////////////////////////////////////////////////////
// USVCameraMode

USVCameraMode::USVCameraMode()
{
	FieldOfView = 80.f;
	ViewPitchMin = -89.f;
	ViewPitchMax = 60.f;

	BlendTime = 0.5f;
	BlendFunction = ESVCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.f;
	BlendAlpha = 1.f;
	BlendWeight = 1.f;
}

USVCameraComponent* USVCameraMode::GetSVCameraComponent() const
{
	return CastChecked<USVCameraComponent>(GetOuter());
}

UWorld* USVCameraMode::GetWorld() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

AActor* USVCameraMode::GetTargetActor() const
{
	const USVCameraComponent* SVCameraComponent = GetSVCameraComponent();

	return SVCameraComponent->GetTargetActor();
}

const FSVCameraModeView& USVCameraMode::GetCameraModeView() const
{
	return View;
}

float USVCameraMode::GetBlendTime() const
{
	return BlendTime;
}

float USVCameraMode::GetBlendWeight() const
{
	return BlendWeight;
}

void USVCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.f, 1.f);

	const float InvExponent = (BlendExponent > 0.f) ? (1.f / BlendExponent) : 1.f;

	switch (BlendFunction)
	{
	case ESVCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	case ESVCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.f, 1.f, BlendWeight, InvExponent);
		break;

	case ESVCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.f, 1.f, BlendWeight, InvExponent);
		break;

	case ESVCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.f, 1.f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("Invalid BlendFunction"));
		break;
	}
}

void USVCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateBlending(DeltaTime);
}

FVector USVCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check (TargetActor);

	return TargetActor->GetActorLocation();
}

FRotator USVCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check (TargetActor);

	return TargetActor->GetActorRotation();
}

void USVCameraMode::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();

	View.Location = PivotLocation;
	View.FieldOfView = FieldOfView;

	const AActor* TargetActor = GetTargetActor();
	check (TargetActor);
	const APawn* TargetPawn = Cast<APawn>(TargetActor);
	check (TargetPawn);
	const APlayerController* TargetController = Cast<APlayerController>(TargetPawn->GetController());
	check (TargetController);
	
	FRotator DesiredRotation = TargetController->GetControlRotation();
	DesiredRotation.Pitch = FMath::ClampAngle(DesiredRotation.Pitch, ViewPitchMin, ViewPitchMax);
	View.ControlRotation = DesiredRotation;

	const FRotator CurrentRotation = TargetController->PlayerCameraManager->GetCameraRotation();
	const FRotator InterpRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, 10.f);
	
	View.Rotation = InterpRotation;
}

void USVCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.f)
	{
		BlendAlpha += DeltaTime / BlendTime;
		BlendAlpha = FMath::Min(BlendAlpha, 1.f);
	}
	else
	{
		BlendAlpha = 1.f;
	}

	const float Exponent = (BlendExponent > 0.f) ? BlendExponent : 1.f;

	switch (BlendFunction)
	{
	case ESVCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;

	case ESVCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.f, 1.f, BlendAlpha, Exponent);
		break;

	case ESVCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.f, 1.f, BlendAlpha, Exponent);
		break;

	case ESVCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.f, 1.f, BlendAlpha, Exponent);
		break;

	default:
		checkf(false, TEXT("Invalid BlendFunction"));
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// USVCameraModeCollection
USVCameraModeCollection::USVCameraModeCollection()
{
	bIsActive = true;
}

void USVCameraModeCollection::ActivateCollection()
{
	if (!bIsActive)
	{
		bIsActive = true;

		for (USVCameraMode* CameraMode : CameraModeCollection)
		{
			check(CameraMode);
			CameraMode->OnActivation();
		}
	}
}

void USVCameraModeCollection::DeactivateCollection()
{
	if (bIsActive)
	{
		bIsActive = false;

		for (USVCameraMode* CameraMode : CameraModeCollection)
		{
			check(CameraMode);
			CameraMode->OnDeactivation();
		}
	}
}

bool USVCameraModeCollection::IsCollectionActivate()
{
	return bIsActive;
}

void USVCameraModeCollection::AddCameraMode(TSubclassOf<USVCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	USVCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 CollectionSize = CameraModeCollection.Num();

	if (CollectionSize > 0 && CameraModeCollection[0] == CameraMode)
	{
		// 이미 해당 CameraMode가 가장 최신 상태인 경우 즉시 반환합니다.
		return;
	}

	int32 ExistingCameraModeIndex = INDEX_NONE;
	float ExistingCameraModeContribution = 1.f;

	// 현재 삽입 중인 CameraMode가 이미 Collection 내에 존재하는지 확인하는 반복문입니다.
	// 블렌드가 자연스럽게 이루어질 수 있도록 기존 CameraMode의 BlendWeight를 그대로 가져오는 과정입니다.
	for (int32 CameraModeIndex = 0; CameraModeIndex < CollectionSize; ++CameraModeIndex)
	{
		if (CameraModeCollection[CameraModeIndex] == CameraMode)
		{
			ExistingCameraModeIndex = CameraModeIndex;
			ExistingCameraModeContribution *= CameraMode->GetBlendWeight();
			break;
		}
		else
		{
			ExistingCameraModeContribution *= (1.f - CameraModeCollection[CameraModeIndex]->GetBlendWeight());
		}
	}

	if (ExistingCameraModeIndex != INDEX_NONE)
	{
		// Collection 내에 동일한 CameraMode가 존재하는 경우 들어오는 분기입니다.
		CameraModeCollection.RemoveAt(ExistingCameraModeIndex);
		CollectionSize--;
	}
	else
	{
		// 존재하지 않는 경우 들어오는 분기입니다.
		ExistingCameraModeContribution = 0.f;
	}

	// 초기 BlendWeight를 결정합니다.
	const bool bShouldBlend = (CameraMode->GetBlendTime() > 0.f) && (CollectionSize > 0);
	const float BlendWeight = bShouldBlend ? ExistingCameraModeContribution : 1.f;

	CameraMode->SetBlendWeight(BlendWeight);

	// 0번 인덱스에 CameraMode를 삽입합니다.
	CameraModeCollection.Insert(CameraMode, 0);

	// Collection에 남아있는 CameraMode 중 가장 먼저 들어온 CameraMode의 BlendWeight가 1.f임을 보장합니다.
	CameraModeCollection.Last()->SetBlendWeight(1.f);

	// 새로 추가된 CameraMode라면 들어가는 분기입니다.
	if (ExistingCameraModeIndex == INDEX_NONE)
	{
		CameraMode->OnActivation();
	}
}

bool USVCameraModeCollection::EvaluateCollection(float DeltaTime, FSVCameraModeView& OutCameraModeView)
{
	if (!bIsActive)
	{
		return false;
	}

	UpdateCollection(DeltaTime);
	BlendCollection(OutCameraModeView);

	return true;
}

USVCameraMode* USVCameraModeCollection::GetCameraModeInstance(TSubclassOf<USVCameraMode> CameraModeClass)
{
	// CameraMode 인스턴스를 매번 생성하지 않고 캐싱해서 재사용하는 구조입니다.
	// CameraMode는 변경되는 값이 많지 않으며, 빠르게 추가/제거될 것으로 예상되는 로직이기 때문에 캐싱하는 편이 효율적입니다.
	check(CameraModeClass);

	for (USVCameraMode* CameraMode : CameraModeInstances)
	{
		if (CameraMode != nullptr && CameraMode->GetClass() == CameraModeClass)
		{
			return CameraMode;
		}
	}

	USVCameraMode* NewCameraMode = NewObject<USVCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void USVCameraModeCollection::UpdateCollection(float DeltaTime)
{
	const int32 CollectionSize = CameraModeCollection.Num();
	if (CollectionSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	for (int32 CollectionIndex = 0; CollectionIndex < CollectionSize; ++CollectionIndex)
	{
		USVCameraMode* CameraMode = CameraModeCollection[CollectionIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(DeltaTime);

		// BlendWeight가 1.f인 CameraMode보다 우선순위가 낮은 경우, 더이상 View에 영향을 줄 수 없으므로 Collection에서 제거합니다.
		if (CameraMode->GetBlendWeight() >= 1.f)
		{
			RemoveIndex = CollectionIndex + 1;
			RemoveCount = CollectionSize - RemoveIndex;
			break;
		}
	}

	if (RemoveCount > 0)
	{
		for (int32 CollectionIndex = RemoveIndex; CollectionIndex < CollectionSize; ++CollectionIndex)
		{
			USVCameraMode* CameraMode = CameraModeCollection[CollectionIndex];
			check(CameraMode);

			CameraMode->OnDeactivation();
		}

		CameraModeCollection.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void USVCameraModeCollection::BlendCollection(FSVCameraModeView& OutCameraModeView)
{
	const int32 CollectionSize = CameraModeCollection.Num();
	if (CollectionSize <= 0)
	{
		return;
	}

	const USVCameraMode* CameraMode = CameraModeCollection[CollectionSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->GetCameraModeView();

	for (int32 CollectionIndex = CollectionSize - 2; CollectionIndex >= 0; --CollectionIndex)
	{
		CameraMode = CameraModeCollection[CollectionIndex];
		check(CameraMode);

		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}