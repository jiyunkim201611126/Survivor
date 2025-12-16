// KJY

#include "DamageSlateManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Survivor/UI/Slate/SDamageTextCanvas.h"

void UDamageSlateManagerSubsystem::Deinitialize()
{
	// 뷰포트에서 위젯을 제거합니다.
	if (GEngine && GEngine->GameViewport && DamageTextCanvas.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(DamageTextCanvas.ToSharedRef());
	}
	
	Super::Deinitialize();
}

void UDamageSlateManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Subsystem의 Initialize 타이밍에 GameViewport가 초기화됐으리란 보장이 없습니다.
	// 따라서 Tick에서 ViewportWidget을 추가하는 방식을 사용합니다.
	if (!bWidgetInitialized && GEngine && GEngine->GameViewport)
	{
		// 슬레이트 위젯을 생성합니다.
		DamageTextCanvas = SNew(SDamageTextCanvas, this);
		
		// 생성한 위젯을 뷰포트에 추가합니다.
		GEngine->GameViewport->AddViewportWidgetContent(DamageTextCanvas.ToSharedRef(), 0);

		bWidgetInitialized = true;
	}

	const APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController)
	{
		ActiveDamageNumbers.Empty();
		return;
	}

	// 현재 화면에 표시될 가능성이 있는 Damage Text들을 순회하며 값을 갱신합니다.
	for (int32 i = ActiveDamageNumbers.Num() - 1; i >= 0; --i)
	{
		FDamageInfo& Info = ActiveDamageNumbers[i];

		Info.LifeTime -= DeltaTime;

		if (Info.LifeTime <= 0.f)
		{
			ActiveDamageNumbers.RemoveAt(i);
		}
		else
		{
			Info.VerticalOffset += DeltaTime * 30.f;

			Info.bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, Info.HitLocation, Info.ScreenPosition, true);
		}
	}
}

void UDamageSlateManagerSubsystem::ShowDamageNumber(const float InDamageAmount, const FVector& InHitLocation)
{
	FDamageInfo NewInfo;
	if (InDamageAmount <= 0.f)
	{
		NewInfo.DamageAmountText = TEXT("No Damage!");
	}
	else
	{
		const float RoundDamage = FMath::RoundToFloat(InDamageAmount);
		NewInfo.DamageAmountText = FString::SanitizeFloat(RoundDamage, 0);
	}
	NewInfo.HitLocation = InHitLocation;
	ActiveDamageNumbers.Add(NewInfo);
}

TArray<FDamageInfo>* UDamageSlateManagerSubsystem::GetActiveDamageNumbers()
{
	return &ActiveDamageNumbers;
}

TStatId UDamageSlateManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDamageSlateManagerSubsystem, STATGROUP_Game);
}
