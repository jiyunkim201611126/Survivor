// KJY


#include "SVWidgetController.h"

#include "Survivor/Player/SVPlayerController.h"
#include "Survivor/Player/SVPlayerState.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/SVAttributeSet.h"

void USVWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	SVPlayerController = Cast<ASVPlayerController>(WidgetControllerParams.PlayerController);
	SVPlayerState = Cast<ASVPlayerState>(WidgetControllerParams.PlayerState);
	SVAbilitySystemComponent = Cast<USVAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	SVAttributeSet = Cast<USVAttributeSet>(WidgetControllerParams.AttributeSet);
}

void USVWidgetController::BindCallbacksToDependencies()
{
}

void USVWidgetController::BroadcastInitialValue()
{
}

ASVPlayerController* USVWidgetController::GetSVPC()
{
	return SVPlayerController;
}

ASVPlayerState* USVWidgetController::GetSVPS()
{
	return SVPlayerState;
}

USVAbilitySystemComponent* USVWidgetController::GetSVASC()
{
	return SVAbilitySystemComponent;
}

USVAttributeSet* USVWidgetController::GetSVAS()
{
	return SVAttributeSet;
}
