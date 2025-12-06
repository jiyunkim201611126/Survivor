// KJY

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SVAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class SURVIVOR_API ASVAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASVAIController();

public:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
