// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "SVPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class SURVIVOR_API ASVPlayerController : public APlayerController, public ISVCameraAssistInterface
{
	GENERATED_BODY()

public:
	//~ Begin ISVCameraAssistInterface
	virtual void StartHide_Implementation() override;
	virtual void EndHide_Implementation() override;
	//~ End of ISVCameraAssistInterface

	void PauseOrRestartGame();

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End of AActor Interface
	
	//~ Begin AController Interface
	virtual void SetupInputComponent() override;
	//~ End of AController Interface

private:
	void ToggleHideActor(AActor* ActorToHide, bool bShouldHide) const;

private:
	bool bNowHiding = false;
	bool bNowPlaying = true;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ESCAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
};
