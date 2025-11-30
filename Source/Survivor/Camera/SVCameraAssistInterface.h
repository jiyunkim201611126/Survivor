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
 * 해당 인터페이스는 PlayerController와 'Attach된 Actor 중 렌더링을 끌 Actor를 가진 Actor'가 상속받습니다.
 * 예시로 캐릭터가 무기를 쥐고 있다면, 무기 Actor를 Hide Target 목록에 추가하기 위해 해당 인터페이스를 캐릭터가 상속받아야 합니다.
 * CameraMode에 의해 PlayerController의 해당 인터페이스의 함수가 호출되면, PlayerController는 숨길 액터 목록을 Pawn으로부터 가져와 화면에서 숨깁니다. 
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

	// PlayerController가 오버라이드해 Actor들을 숨기는 트리거 함수입니다.
	virtual void OnCameraPenetratingTarget() { }
};
