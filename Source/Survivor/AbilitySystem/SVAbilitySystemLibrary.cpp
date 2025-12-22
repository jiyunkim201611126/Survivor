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

void USVAbilitySystemLibrary::CalculateEvenlyRotatedVectors(FVector& NormalizedForwardVector, const int32 NumOfVectors, const float Angle, const float Length, TArray<FVector>& OutVectors)
{
	// 잘못된 매개변수가 들어오면 바로 return합니다.
	if (NumOfVectors < 1)
	{
		return;
	}

	// 결과 Vector들에 각각 Length를 곱하기보다는, ForwardVector에 Length를 먼저 곱합니다.
	NormalizedForwardVector *= Length;
	OutVectors.Reserve(NumOfVectors);

	if (NumOfVectors == 1)
	{
		OutVectors.Emplace(NormalizedForwardVector);
		return;
	}

	// 왼쪽 시작 각도와 각도 변화량을 계산합니다.
	const float StartAngle = -Angle / 2.f;
	const float DeltaAngle = Angle / (NumOfVectors - 1);

	// 위 각도 계산 결과를 토대로 Vector들을 계산합니다.
	for (int32 Index = 0; Index < NumOfVectors; ++Index)
	{
		const float CurrentAngle = StartAngle + (DeltaAngle * Index);
		const FVector RotatedVector = NormalizedForwardVector.RotateAngleAxis(CurrentAngle, FVector::UpVector);
		OutVectors.Emplace(RotatedVector);
	}
}
