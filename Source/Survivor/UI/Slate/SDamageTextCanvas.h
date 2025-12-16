// KJY

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDamageSlateManagerSubsystem;

/**
 * 실제로 화면에 Damage를 표시하는 Slate Widget입니다.
 * UObject 기반이 아니기 때문에 Blueprint 호환이 어려우나, 같은 이유로 매우 가볍습니다.
 * 예를 들어 100개의 Damage Text를 표시해야 한다면, UUserWidget 기반인 경우 Damage Text Widget을 100개를 생성해야 합니다.
 * 하지만 이 클래스를 사용해 구현하면 1개의 Slate Widget의 OnPaint 함수에서 100개의 텍스트를 그리게 됩니다.
 */
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
