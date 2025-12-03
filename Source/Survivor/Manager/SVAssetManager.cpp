// KJY


#include "SVAssetManager.h"

#include "SVGameplayTags.h"
#include "AbilitySystemGlobals.h"

USVAssetManager& USVAssetManager::Get()
{
	check(GEngine);
	USVAssetManager* AuraAssetManager = Cast<USVAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void USVAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 전역으로 선언되어있는 GameplayTags 인스턴스를 초기화합니다.
	FSVGameplayTags::InitializeNativeGameplayTags();

	// TargetData 사용 및 커스텀 Context를 사용하기 위해 반드시 호출해줘야 하는 함수입니다.
	UAbilitySystemGlobals::Get().InitGlobalData();
}
