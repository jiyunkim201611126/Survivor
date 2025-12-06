// KJY

#pragma once

#include "CoreMinimal.h"
#include "Survivor/AbilitySystem/Data/AbilityInfo.h"
#include "UObject/Object.h"
#include "SVWidgetController.generated.h"

class UPlayerAttributeSet;
class USVAbilitySystemComponent;
class ASVPlayerState;
class ASVPlayerController;
class UAttributeSet;
class UAbilitySystemComponent;

// 위젯 컨트롤러의 멤버 변수 초기화를 간편화하는 구조체
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FSVAbilityInfo&, Info);

/**
 * MVVM 패턴의 VM을 담당하는 Widget Controller입니다.
 * MVVM 패턴은 UI를 관리하는 패턴 중 GAS 기반의 프로젝트와 가장 잘 어울리는 패턴이라는 게 정설입니다.
 * HUD를 포함해 UI와 관련된 모든 객체(ASC, AttributeSet, PlayerController 등)이 모두 생성되었다고 판단되는 순간, HUD에 의해 생성되는 객체입니다.
 * 생성 이후 즉시 관련 객체들을 할당받으며, 그 객체들에게 콜백 함수를 바인드합니다.
 * 바인드를 마치면 모든 관련 Widget들에게 뿌려지게 되며, Widget들은 이를 변수로 할당하고 마찬가지로 콜백 함수를 바인드합니다.
 *
 * 완료 후 로직 흐름은 이렇습니다.
 * UI에 반영되어야 하는 변경 사항 발생(Ability 사용, Attribute 변경 등) -> Widget Controller의 콜백 함수 호출 -> Widget의 콜백 함수 호출 -> 반영 완료
 */

UCLASS(Abstract)
class SURVIVOR_API USVWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);
	virtual void BindCallbacksToDependencies();
	
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValue();

	ASVPlayerController* GetSVPC();
	ASVPlayerState* GetSVPS();
	USVAbilitySystemComponent* GetSVASC();
	UPlayerAttributeSet* GetSVPlayerAS();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<ASVPlayerController> SVPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<ASVPlayerState> SVPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<USVAbilitySystemComponent> SVAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UPlayerAttributeSet> SVAttributeSet;
};
