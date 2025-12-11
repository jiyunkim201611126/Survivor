// KJY

#pragma once

#include "CoreMinimal.h"
#include "Survivor/UI/Widget/SVUserWidget.h"
#include "SVProgressBar.generated.h"

class UProgressBar;

/**
 * ProgressBar 2개로 구현되는 프로젝트 기본 ProgressBar입니다.
 * Front는 값이 바로 변경되며, Ghost는 타이머를 통해 Front를 뒤따라갑니다.
 * 3개의 함수를 통해 Ghost가 조정되며, 블루프린트에서 오버라이드해 사용하는 것도 가능합니다.
 */

UCLASS()
class SURVIVOR_API USVProgressBar : public USVUserWidget
{
	GENERATED_BODY()

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeDestruct() override;
	//~ End of UUserWidget Interface
	
	UFUNCTION(BlueprintCallable)
	void SetProgressBarStyle(const FProgressBarStyle& FrontProgressBarStyle, const FProgressBarStyle& GhostProgressBarStyle);

	UFUNCTION(BlueprintCallable)
	void SetFillColorAndOpacity(const FLinearColor& InLinearColor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetBarPercent(const float InPercent, bool bShouldInterp = true);

	UFUNCTION(BlueprintNativeEvent)
	void BarPercentSet();

	UFUNCTION(BlueprintNativeEvent)
	void InterpGhostBar();

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget = "true"))
	TObjectPtr<UProgressBar> FrontProgressBar;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget = "true"))
	TObjectPtr<UProgressBar> GhostProgressBar;

	FTimerHandle GhostPercentSetTimerHandle;
	FTimerHandle PercentInterpTimerHandle;

	float GhostPercentTarget = 0.5f;
	float GhostStartDelay = 1.f;
	float GhostInterpDelay = 0.05f;
	float GhostInterpSpeed = 5.f;
};
