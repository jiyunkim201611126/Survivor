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

TArray<FVector> USVAbilitySystemLibrary::CalculateEvenlyRotatedVectors(const FVector& OriginLocation, FVector& NormalizedForwardVector, const int32 NumOfVectors, const float Degree, const float Length)
{
	TArray<FVector> OutVectors;
	OutVectors.Reserve(NumOfVectors);

	if (NumOfVectors < 1)
	{
		return OutVectors;
	}

	// 결과 Vector들 각각 Length를 곱하기보다는, ForwardVector에 Vector의 길이를 먼저 곱합니다.
	NormalizedForwardVector *= Length;

	// 왼쪽 시작점 Vector와 각도 변화량을 구합니다.
	const FVector LeftDirection = NormalizedForwardVector.RotateAngleAxis(-Degree / 2.f, FVector::UpVector);
	const float DeltaDegree = NumOfVectors > 1 ? Degree / (NumOfVectors - 1) : 0.f;
	// 이미 시작점 Vector는 구했으므로, 바로 OutVector에 넣습니다.
	OutVectors.Emplace(LeftDirection + OriginLocation);

	// 시작점은 바로 위에서 넣었으므로, 1번 Index부터 시작합니다.
	for (int32 VectorIndex = 1; VectorIndex < NumOfVectors; ++VectorIndex)
	{
		// 왼쪽 시작점부터 각도 변화량만큼 더해가며 결과 Vector들을 구한 뒤, 기준 위치와 더해줍니다.
		const FVector Direction = NumOfVectors > 1 ? LeftDirection.RotateAngleAxis(DeltaDegree * VectorIndex, FVector::UpVector) : NormalizedForwardVector;
		OutVectors.Emplace(Direction + OriginLocation);
	}

	return OutVectors;
}
