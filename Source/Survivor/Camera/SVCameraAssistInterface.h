// KJY

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SVCameraAssistInterface.generated.h"

UINTERFACE()
class USVCameraAssistInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 해당 인터페이스는 PlayerController와 카메라가 가까워졌을 때 Hide될 Actor가 상속받습니다.
 * Attach된 Actor를 Hide Target 목록에 추가하기 위해 해당 인터페이스를 Pawn 등이 상속받아야 합니다.
 * CameraMode에 의해 PlayerController의 해당 인터페이스의 함수가 호출되면, PlayerController는 숨길 액터들의 StartHide 및 EndHide를 호출해줍니다.
 */
class SURVIVOR_API ISVCameraAssistInterface
{
	GENERATED_BODY()

public:
	// 추가적으로 숨길 Actor를 반환하는 함수입니다.
	// 예) 손에 쥔 무기 등
	virtual TArray<AActor*> GetAdditionalHideTargetActors() const
	{
		return TArray<AActor*>();
	}

	// Actor들을 숨기는 트리거 함수입니다.
	UFUNCTION(BlueprintNativeEvent)
	void StartHide();
	UFUNCTION(BlueprintNativeEvent)
	void EndHide();
};
