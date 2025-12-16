// KJY

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DamageSlateManagerSubsystem.generated.h"

class SDamageTextCanvas;

USTRUCT()
struct FDamageInfo
{
	GENERATED_BODY()

	FString DamageAmountText = TEXT("");
	
	FVector HitLocation = FVector::ZeroVector;

	float LifeTime = 1.5f;

	float VerticalOffset = 0.f;

	FVector2D ScreenPosition = FVector2D::ZeroVector;

	bool bIsOnScreen = false;
};

/**
 * Damage와 관련된 Slate의 관리를 담당하는 클래스입니다.
 * Damage 표시를 위해 각각의 객체가 스스로 동작하기보단, Slate의 특성을 활용해 1개의 객체에서 여러 번의 Paint 함수 호출로 최적화합니다.
 */
UCLASS()
class SURVIVOR_API UDamageSlateManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin UWorldSubsystem Interface
	virtual void Deinitialize() override;
	//~ End of UWorldSubsystem Interface
	
	//~ Begin FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	// Tick 함수의 실행 시간을 stat game에 표시해주는 프로파일링용 함수입니다.
	virtual TStatId GetStatId() const override;
	//~ End of FTickableGameObject Interface

	void ShowDamageNumber(const float InDamageAmount, const FVector& InHitLocation);

	TArray<FDamageInfo>* GetActiveDamageNumbers();

private:
	UPROPERTY()
	TArray<FDamageInfo> ActiveDamageNumbers;

	TSharedPtr<SDamageTextCanvas> DamageTextCanvas;

	uint8 bWidgetInitialized : 1 = false;
};
