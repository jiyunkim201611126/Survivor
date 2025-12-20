// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVEnemy.h"
#include "SVFlipbookEnemy.generated.h"

class UCameraComponent;
class UPaperFlipbook;
class UPaperFlipbookComponent;

// 캐릭터의 어느 부분을 그려야 하는지 나타내는 Enum입니다.
UENUM()
enum class EEnemyDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

UCLASS()
class SURVIVOR_API ASVFlipbookEnemy : public ASVEnemy
{
	GENERATED_BODY()

public:
	ASVFlipbookEnemy(const FObjectInitializer& ObjectInitializer);

	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End of AActor Interface

	//~ Begin ISVCameraAssistInterface
	virtual void StartHide_Implementation() override;
	virtual void EndHide_Implementation() override;
	//~ End of ISVCameraAssistInterface

private:
	void SetLocalCameraComponent();
	void UpdateFlipbookImage();
	void UpdateFlipbookComponentDirection() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flipbook", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperFlipbookComponent> PaperFlipbookComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipbook", meta = (AllowPrivateAccess = "true"))
	TMap<EEnemyDirection, TObjectPtr<UPaperFlipbook>> Flipbooks;

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CameraComponent;

	EEnemyDirection CurrentDirection = EEnemyDirection::Front;
};
