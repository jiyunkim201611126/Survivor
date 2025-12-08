// KJY

#include "SVPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Survivor/UI/HUD/SVHUD.h"

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

void ASVPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void ASVPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(ESCAction, ETriggerEvent::Started, this, &ThisClass::PauseOrRestartGame);
}

void ASVPlayerController::PauseOrRestartGame()
{
	if (const ASVHUD* SVHUD = Cast<ASVHUD>(GetHUD()))
	{
		const float TimeDilation = bNowPlaying ? 0.f : 1.f;
	
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
		SVHUD->SetOverlayVisibleState(!bNowPlaying);

		bShowMouseCursor = bNowPlaying;
		bNowPlaying = !bNowPlaying;

		if (bShowMouseCursor)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputModeData.SetHideCursorDuringCapture(false);
			SetInputMode(InputModeData);
		}
		else
		{
			FInputModeGameOnly InputModeData;
			SetInputMode(InputModeData);
		}
	}
}
