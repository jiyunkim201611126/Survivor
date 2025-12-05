// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "SVPlayerController.generated.h"

UCLASS()
class SURVIVOR_API ASVPlayerController : public APlayerController, public ISVCameraAssistInterface
{
	GENERATED_BODY()

public:
	//~ Begin ISVCameraAssistInterface
	virtual void StartHide_Implementation() override;
	virtual void EndHide_Implementation() override;
	//~ End of ISVCameraAssistInterface

private:
	void ToggleHideActor(AActor* ActorToHide, bool bShouldHide) const;

private:
	bool bNowHiding = false;
};
