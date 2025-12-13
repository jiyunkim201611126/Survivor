// KJY

#include "SDamageTextCanvas.h"

#include "Engine/Font.h"
#include "Fonts/FontMeasure.h"
#include "Survivor/Manager/DamageSlateManagerSubsystem.h"

void SDamageTextCanvas::Construct(const FArguments& InArgs, UDamageSlateManagerSubsystem* InManager)
{
	Manager = InManager;
	SetVisibility(EVisibility::HitTestInvisible);
	const FString FontPath = TEXT("/Game/Assets/Fonts/David_Libre/DavidLibre-Medium_Font.DavidLibre-Medium_Font");
	Font = LoadObject<UFont>(nullptr, *FontPath);
}

int32 SDamageTextCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 ResultLayerID = LayerId;

	const float DPI_Scale = AllottedGeometry.GetAccumulatedLayoutTransform().GetScale();
	if (!Manager.IsValid() || FMath::IsNearlyZero(DPI_Scale))
	{
		return ResultLayerID;
	}
	
	FSlateFontInfo FontInfo;
	if (Font)
	{
		FontInfo = FSlateFontInfo(Font, 24.f);
	}
	else
	{
		FontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
		FontInfo.Size = 24.f;
	}

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	for (const FDamageInfo& Info : *Manager->GetActiveDamageNumbers())
	{
		// 화면에 표시될 때만 그립니다.
		if (Info.bIsOnScreen)
		{
			// 그려질 텍스트의 크기를 미리 계산합니다.
			const FVector2D TextSize = FontMeasure->Measure(Info.DamageAmountText, FontInfo);

			// 원래 위치에서 텍스트 너비의 절반만큼 왼쪽으로 이동하여 중앙을 맞춥니다.
			FVector2D DrawPosition = Info.ScreenPosition / DPI_Scale;
			DrawPosition.X -= TextSize.X * 0.5f;
			// 점점 위쪽으로 이동하는 애니메이션 추가합니다.
			DrawPosition.Y -= Info.VerticalOffset;

			float Alpha = Info.LifeTime > 0.5f ? 1.f : Info.LifeTime / 0.5f;
			FLinearColor TextColor = FLinearColor(1.f, 0.8f, 0.2f, Alpha);

			FSlateDrawElement::MakeText(
				OutDrawElements,
				++ResultLayerID,
				AllottedGeometry.ToPaintGeometry(FVector2D(1,1), FSlateLayoutTransform(DrawPosition)),
				Info.DamageAmountText,
				FontInfo,
				ESlateDrawEffect::None,
				TextColor
				);
		}
	}

	return ResultLayerID;
}
