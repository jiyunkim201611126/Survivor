// KJY

#include "OverlayWidget.h"

#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Survivor/UI/WidgetController/OverlayWidgetController.h"

void UOverlayWidget::WidgetControllerSet_Implementation()
{
	StatusBars->SetWidgetController(WidgetController);
	InGameMenu->SetWidgetController(WidgetController);

	UOverlayWidgetController* OverlayWidgetController = Cast<UOverlayWidgetController>(WidgetController);
	OverlayWidgetController->OnXPChangedDelegate.AddDynamic(this, &ThisClass::OnXPChanged);
	OverlayWidgetController->BroadcastInitialValue();
}

void UOverlayWidget::SetOverlayVisibleState(const bool bNowPlaying) const
{
	if (bNowPlaying)
	{
		AttributeList->SetVisibility(ESlateVisibility::Collapsed);
		InGameMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		AttributeList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		InGameMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UOverlayWidget::OnXPChanged(const int32 InCurrentXP, const int32 InMaxXP)
{
	const float XPBarPercent = UKismetMathLibrary::SafeDivide(InCurrentXP, InMaxXP);
	XPProgressBar->SetPercent(XPBarPercent);
	XPValueTextBlock->SetText(FText::Format(FText::FromString("{0} / {1}"), InCurrentXP, InMaxXP));
}
