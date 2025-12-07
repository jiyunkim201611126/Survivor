// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVUserWidget.h"
#include "OverlayWidget.generated.h"

class UButton;
class UTextBlock;
class UProgressBar;

UCLASS()
class SURVIVOR_API UOverlayWidget : public USVUserWidget
{
	GENERATED_BODY()

public:
	void SetOverlayVisibleState(const bool bNowPlaying) const;

protected:
	//~ Begin USVUserWidget Interface
	virtual void WidgetControllerSet_Implementation() override;
	//~ End of USVUserWidget Interface

private:
	UFUNCTION()
	void OnXPChanged(const int32 InCurrentXP, const int32 InMaxXP);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USVUserWidget> AttributeList;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USVUserWidget> StatusBars;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> XPProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> XPValueTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USVUserWidget> InGameMenu;
};
