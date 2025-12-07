// KJY

#include "SVUserWidget.h"

void USVUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

void USVUserWidget::WidgetControllerSet_Implementation()
{
}
