// KJY

#include "SVPlayerController.h"

void ASVPlayerController::StartHide_Implementation()
{
	if (bNowHiding)
	{
		return;
	}
	
	AActor* ViewTarget = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;

	if (ViewTarget)
	{
		// ViewTarget과 그에 붙어있는 모든 Actor를 Hide하는 로직을 시작합니다.
		ToggleHideActor(ViewTarget, true);
	}

	bNowHiding = true;
}

void ASVPlayerController::EndHide_Implementation()
{
	if (!bNowHiding)
	{
		return;
	}
	
	AActor* ViewTarget = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;

	if (ViewTarget)
	{
		// ViewTarget과 그에 붙어있는 모든 Actor를 Hide하는 로직을 시작합니다.
		ToggleHideActor(ViewTarget, false);
	}

	bNowHiding = false;
}

void ASVPlayerController::ToggleHideActor(AActor* ActorToHide, const bool bShouldHide) const
{
	if (const ISVCameraAssistInterface* CameraAssistInterface = Cast<ISVCameraAssistInterface>(ActorToHide))
	{
		// Attach된 Actor도 Hide해야 하므로 재귀적으로 탐색합니다.
		TArray<AActor*> AdditionalHideTargetActors = CameraAssistInterface->GetAdditionalHideTargetActors();
		for (AActor* TargetActor : AdditionalHideTargetActors)
		{
			ToggleHideActor(TargetActor, bShouldHide);
		}

		bShouldHide ? Execute_StartHide(ActorToHide) : Execute_EndHide(ActorToHide);
	}
}
