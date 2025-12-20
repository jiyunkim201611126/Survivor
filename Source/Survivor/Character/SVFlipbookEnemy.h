// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVEnemy.h"
#include "SVFlipbookEnemy.generated.h"

class UPaperFlipbookComponent;

UCLASS()
class SURVIVOR_API ASVFlipbookEnemy : public ASVEnemy
{
	GENERATED_BODY()

public:
	ASVFlipbookEnemy(const FObjectInitializer& ObjectInitializer);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperFlipbookComponent> PaperFlipbookComponent;
};
