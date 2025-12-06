// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVCharacterBase.h"
#include "Survivor/AI/SVAIController.h"
#include "SVEnemy.generated.h"

UCLASS()
class SURVIVOR_API ASVEnemy : public ASVCharacterBase
{
	GENERATED_BODY()

public:
	ASVEnemy();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	//~ End of APawn Interface

	//~ Begin ASVCharacterBase Interface
	virtual void InitAbilityActorInfo() override;
	virtual void StartHide_Implementation() override;
	virtual void EndHide_Implementation() override;
	//~ End of ASVCharacterBase Interface

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlaySpawnAnimation();

public:
	UPROPERTY()
	TObjectPtr<ASVAIController> SVAIController;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
