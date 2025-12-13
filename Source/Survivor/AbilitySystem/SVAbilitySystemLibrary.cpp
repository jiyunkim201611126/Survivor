// KJY

#include "SVAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Survivor/Player/SVPlayerState.h"
#include "Survivor/UI/HUD/SVHUD.h"
#include "Survivor/UI/WidgetController/SVWidgetController.h"

bool USVAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWidgetControllerParams, ASVHUD*& OutSVHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		ASVPlayerState* PS = PC->GetPlayerState<ASVPlayerState>();
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		UAttributeSet* AS = PS->GetAttributeSet();
		
		OutWidgetControllerParams.PlayerController = PC;
		OutWidgetControllerParams.PlayerState = PS;
		OutWidgetControllerParams.AbilitySystemComponent = ASC;
		OutWidgetControllerParams.AttributeSet = AS;
		OutSVHUD = Cast<ASVHUD>(PC->GetHUD());
		
		return true;
	}
	return false;
}

UOverlayWidgetController* USVAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	ASVHUD* SVHUD = nullptr;
	const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, SVHUD);

	if (bSuccessfulParams)
	{
		return SVHUD->GetOverlayWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

UAttributeMenuWidgetController* USVAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	ASVHUD* SVHUD = nullptr;
	const bool bSuccessfulParams = MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams, SVHUD);

	if (bSuccessfulParams)
	{
		return SVHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}

	return nullptr;
}

FDamageDataContext USVAbilitySystemLibrary::GetDamageData(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FSVGameplayEffectContext* AuraEffectContext = static_cast<const FSVGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDamageData();
	}
	return FDamageDataContext();
}

void USVAbilitySystemLibrary::SetKnockbackMagnitude(FGameplayEffectContextHandle& EffectContextHandle, const float Magnitude)
{
	if (FSVGameplayEffectContext* SVEffectContext = static_cast<FSVGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		SVEffectContext->SetKnockbackMagnitude(Magnitude);
	}
}

TArray<AActor*> USVAbilitySystemLibrary::GetActorsFromContext(FGameplayEffectContextHandle& EffectContextHandle)
{
	TArray<AActor*> Actors;
	for (auto Element : EffectContextHandle.GetActors())
	{
		if (Element.IsValid())
		{
			Actors.Add(Element.Get());
		}
	}
	return Actors;
}
