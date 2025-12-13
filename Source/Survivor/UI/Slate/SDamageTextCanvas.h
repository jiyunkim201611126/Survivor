// KJY

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDamageSlateManagerSubsystem;

class SURVIVOR_API SDamageTextCanvas : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDamageTextCanvas) {}
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UDamageSlateManagerSubsystem* InManager);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	TWeakObjectPtr<UDamageSlateManagerSubsystem> Manager;

	UFont* Font = nullptr;
};
