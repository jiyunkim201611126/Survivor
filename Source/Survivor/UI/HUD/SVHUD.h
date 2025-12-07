// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SVHUD.generated.h"

struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
class UOverlayWidget;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class SURVIVOR_API ASVHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitHUD(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WidgetControllerParams);

	void SetOverlayVisibleState(bool bNowPlaying) const;
	
private:
	UPROPERTY()
	TObjectPtr<UOverlayWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
