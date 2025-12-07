// KJY

#include "SVProgressBar.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

void USVProgressBar::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(GhostPercentSetTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(PercentInterpTimerHandle);
	}
	
	Super::NativeDestruct();
}

void USVProgressBar::SetProgressBarStyle(const FProgressBarStyle& FrontProgressBarStyle, const FProgressBarStyle& GhostProgressBarStyle)
{
	FrontProgressBar->SetWidgetStyle(FrontProgressBarStyle);
	GhostProgressBar->SetWidgetStyle(GhostProgressBarStyle);
}

void USVProgressBar::SetFillColorAndOpacity(const FLinearColor& InLinearColor)
{
	FrontProgressBar->SetFillColorAndOpacity(InLinearColor);
	GhostProgressBar->SetFillColorAndOpacity(InLinearColor);
}

void USVProgressBar::SetBarPercent_Implementation(const float InPercent, bool bShouldInterp)
{
	FrontProgressBar->SetPercent(InPercent);

	if (bShouldInterp)
	{
		GetWorld()->GetTimerManager().SetTimer(
			GhostPercentSetTimerHandle,
			this,
			&USVProgressBar::BarPercentSet,
			GhostStartDelay,
			false);
	}
	else
	{
		GhostProgressBar->SetPercent(InPercent);
	}
}

void USVProgressBar::BarPercentSet_Implementation()
{
	GhostPercentTarget = FrontProgressBar->GetPercent();

	GetWorld()->GetTimerManager().ClearTimer(PercentInterpTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		PercentInterpTimerHandle,
		this,
		&USVProgressBar::InterpGhostBar,
		GhostInterpDelay,
		true);
}

void USVProgressBar::InterpGhostBar_Implementation()
{
	const float CurrentGhostBarPercent = GhostProgressBar->GetPercent();
	const float NextGhostBarPercent = UKismetMathLibrary::FInterpTo(CurrentGhostBarPercent, GhostPercentTarget, GetWorld()->GetDeltaSeconds(), GhostInterpSpeed);
	GhostProgressBar->SetPercent(NextGhostBarPercent);

	if (UKismetMathLibrary::NearlyEqual_FloatFloat(NextGhostBarPercent, GhostPercentTarget))
	{
		GetWorld()->GetTimerManager().ClearTimer(PercentInterpTimerHandle);
	}
}
