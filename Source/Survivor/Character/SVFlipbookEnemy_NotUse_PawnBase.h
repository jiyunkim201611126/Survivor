// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "Survivor/Camera/SVCameraAssistInterface.h"
#include "Survivor/Interface/CombatInterface.h"
#include "Survivor/Interface/EnemyInterface.h"
#include "SVFlipbookEnemy_NotUse_PawnBase.generated.h"

class UPaperFlipbook;
class UFloatingPawnMovement;
class USVAbilitySystemComponent;
class USVAttributeSet;
class UEnemyGASManagerComponent;
class UCapsuleComponent;
class UPaperFlipbookComponent;
class UCameraComponent;
struct FGameplayTag;

UENUM()
enum class EFlipbookDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

/**
 * SkeletalMesh도 사용하지 않고, 움직임이 단순해 CharacterMovementComponent까지 필요하지 않다 생각해 선언된 클래스입니다.
 * 따라서 APawn 클래스를 상속받아 구현을 시도했으며, SVCharacterBase와 SVEnemy에서 수행하고 있던 GAS 관련 로직, 그리고 Flipbook 로직을 모두 갖고 있습니다.
 * 그러나 이동 로직을 구현하는 데에 있어 너무 많은 예외처리(공중 이동, 넉백 관련 처리, NavMesh 기반 움직임 등)가 필요했습니다.
 * 그로 인해 틱마다 라인 트레이스를 수행한다거나, 근처의 NavMesh를 탐색하는 등 생각보다 점점 무거워졌습니다.
 * 결론적으로 이후 또 다른 이동 예외처리(절벽에서 낙하한 캐릭터 추적 등)와 GAS 상호작용 등을 고려해 다시 Character 기반 클래스를 사용하기로 결정했습니다.
 */
UCLASS()
class SURVIVOR_API ASVFlipbookEnemy_NotUse_PawnBase : public APawn, public IAbilitySystemInterface, public ICombatInterface, public ISVCameraAssistInterface, public IEnemyInterface
{
	GENERATED_BODY()

public:
	ASVFlipbookEnemy_NotUse_PawnBase();

	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UCapsuleComponent* GetCombatCapsuleComponent() const override;
	//~ End IAbilitySystemInterface

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~ End of APawn Interface

	void ApplyGravity(const float DeltaTime);
	void UpdateMovement() const;

	virtual void ApplyKnockback(const FVector& KnockbackImpulse) override;
	
	virtual void OnSpawnFromPool() override;

private:
	void InitAbilityActorInfo() const;

	void SetLocalCameraComponent();
	void UpdateFlipbookImage() const;
	void UpdateFlipbookComponentDirection() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPaperFlipbookComponent> PaperFlipbookComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyGASManagerComponent> GASManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USVAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<USVAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flipbook", meta = (AllowPrivateAccess = "true"))
	TMap<EFlipbookDirection, TObjectPtr<UPaperFlipbook>> Flipbooks;

	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CameraComponent;

	float VerticalVelocity = 0.f;
	const float GravityForce = 2000.f;
};
