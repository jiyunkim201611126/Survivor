// KJY

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "SVCameraComponent.generated.h"

class USVCameraMode;
class USVCameraModeCollection;

DECLARE_DELEGATE_RetVal(TSubclassOf<USVCameraMode>, FSVCameraModeSignature);

UCLASS()
class SURVIVOR_API USVCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	USVCameraComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "Camera")
	static USVCameraComponent* FindCameraComponent(const AActor* Actor);

	virtual AActor* GetTargetActor() const;

	void AddFieldOfViewOffset(float FovOffset);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetCameraMode(const TSubclassOf<USVCameraMode> NewCameraMode);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ClearCameraMode();

protected:
	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:
	UPROPERTY()
	TObjectPtr<USVCameraModeCollection> CameraModeCollection;

	// FOV 확대/축소를 원하는 경우 사용하는 변수입니다.
	float FieldOfViewOffset = 0.f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "SVCamera", meta = (AllowPrivateAccess = true))
	TSubclassOf<USVCameraMode> DefaultCameraMode;

	UPROPERTY()
	TSubclassOf<USVCameraMode> DesiredCameraMode = nullptr;
};
