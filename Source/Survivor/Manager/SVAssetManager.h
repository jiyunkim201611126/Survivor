// KJY

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SVAssetManager.generated.h"

UCLASS()
class SURVIVOR_API USVAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static USVAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
