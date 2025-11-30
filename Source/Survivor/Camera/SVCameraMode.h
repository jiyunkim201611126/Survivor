// KJY

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SVCameraMode.generated.h"

class USVCameraComponent;

UENUM(BlueprintType)
enum class ESVCameraModeBlendFunction : uint8
{
	// 직선입니다.
	Linear,
	
	// 시작 시점에서 빠르게 가속 후 천천히 감속하며 도달합니다.
	EaseIn,

	// 시작 시점에서 천천히 가속 후 계속해서 가속합니다.
	EaseOut,

	// 시작 시점에서 천천히 가속 후 천천히 감속하며 도달합니다.
	EaseInOut,
	
	COUNT UMETA(Hidden)
};

struct FSVCameraModeView
{
public:
	FSVCameraModeView();

	void Blend(const FSVCameraModeView& Other, float OtherWeight);

public:
	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/**
 * 카메라의 FOV, 최대 회전각 등을 정의하는 객체입니다.
 * 단순히 단일 객체로서의 역할만이 아니라 다른 CameraMode들과 어떻게 Blend될 것인지도 정의합니다.
 * 해당 클래스는 바로 블루프린트 클래스로 파생될 수 없으며, 상속 후 UCLASS 매크로에 Blueprintable을 선언해야 합니다.
 */
UCLASS(Abstract, NotBlueprintable)
class SURVIVOR_API USVCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	USVCameraMode();

	USVCameraComponent* GetSVCameraComponent() const;

	virtual UWorld* GetWorld() const override;

	AActor* GetTargetActor() const;

	const FSVCameraModeView& GetCameraModeView() const;

	virtual void OnActivation() {};
	virtual void OnDeactivation() {};

	float GetBlendTime() const;
	float GetBlendWeight() const;
	
	void SetBlendWeight(float Weight);

	void UpdateCameraMode(float DeltaTime);

protected:
	// 카메라 회전의 중심이 되는 지점을 반환하는 함수입니다.
	virtual FVector GetPivotLocation() const;
	// 카메라의 정면 방향을 반환하는 함수입니다.
	virtual FRotator GetPivotRotation() const;

	virtual void UpdateView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);

protected:
	FSVCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170"))
	float FieldOfView;
	
	UPROPERTY(EditDefaultsOnly, Category = "View", meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;
	
	UPROPERTY(EditDefaultsOnly, Category = "View", meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// CameraMode의 값들이 모두 반영되기까지 걸리는 시간입니다.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	ESVCameraModeBlendFunction BlendFunction;

	// 0을 초과한 값을 가지며, 곡선의 기울기를 결정하는 변수입니다.
	// 1.f보다 큰 경우 클수록 처음이 더 완만하고, 점점 급격해집니다.
	// 0.f ~ 1.f 사이의 값은 처음이 더 급격하고, 점점 완만해집니다.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// 현재까지 진행된 시간을 나타내는 변수입니다.
	float BlendAlpha;
	// 현재 얼마나 적용되고 있는지를 나타냅니다.
	float BlendWeight;
};

/**
 * 위 클래스 CameraMode의 집합입니다.
 * 매 틱마다 CameraMode의 상태를 업데이트하고, 모든 CameraMode의 수치에 따라 Blend시켜 최종적으로 화면에 표시할 카메라 수치들을 계산합니다.
 */
UCLASS()
class USVCameraModeCollection : public UObject
{
	GENERATED_BODY()

public:
	USVCameraModeCollection();

	void ActivateCollection();
	void DeactivateCollection();

	bool IsCollectionActivate();

	void AddCameraMode(TSubclassOf<USVCameraMode> CameraModeClass);
	
	bool EvaluateCollection(float DeltaTime, FSVCameraModeView& OutCameraModeView);

protected:
	USVCameraMode* GetCameraModeInstance(TSubclassOf<USVCameraMode> CameraModeClass);

	void UpdateCollection(float DeltaTime);
	void BlendCollection(FSVCameraModeView& OutCameraModeView);

protected:
	bool bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<USVCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<USVCameraMode>> CameraModeCollection;
};