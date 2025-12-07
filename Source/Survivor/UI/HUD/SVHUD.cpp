// KJY

#include "SVHUD.h"

#include "Survivor/UI/Widget/OverlayWidget.h"
#include "Survivor/UI/WidgetController/OverlayWidgetController.h"
#include "Survivor/UI/WidgetController/AttributeMenuWidgetController.h"

void ASVHUD::InitHUD(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	check(OverlayWidgetClass);
	check(OverlayWidgetControllerClass);
	check(AttributeMenuWidgetControllerClass);
	
	OverlayWidget = CreateWidget<UOverlayWidget>(GetWorld(), OverlayWidgetClass);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(OverlayWidgetController);
	OverlayWidget->AddToViewport();
}

UOverlayWidgetController* ASVHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* ASVHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

void ASVHUD::SetOverlayVisibleState(const bool bNowPlaying) const
{
	// 지금은 OverlayWidget에게 직접 신호를 보내지만, 추후 다른 위젯도 해당 사항을 알아야 할 경우 WidgetController 객체를 경유합니다.
	OverlayWidget->SetOverlayVisibleState(bNowPlaying);
}
