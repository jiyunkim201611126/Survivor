// KJY

#pragma once

#include "CoreMinimal.h"
#include "SVCharacterBase.h"
#include "SVCharacter.generated.h"

class UGameplayAbility;
class UAttributeSet;
class UPlayerGASManagerComponent;
class UInputAction;
class UInputMappingContext;
class USVCameraComponent;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EMovementType : uint8
{
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint"),
};

UCLASS()
class SURVIVOR_API ASVCharacter : public ASVCharacterBase
{
	GENERATED_BODY()

public:
	ASVCharacter(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void UnPossessed() override;
	//~ End of APawn Interface

	//~ Begin ACharacter Interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ End of ACharacter Interface

	//~ Begin ICombatInterface
	virtual void RegisterPawn(APawn* InPawn) override;
	virtual void UnregisterPawn(APawn* InPawn) override;
	//~ End of ICombatInterface
	
private:
	void UpdateMovement();
	void UpdateRotation() const;
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	virtual void Jump() override;
	void Sprint(const FInputActionValue& InputActionValue);
	void Walk();
	void Strafe();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bJustLanded = false;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector LandVelocity;

private:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USVCameraComponent> CameraComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> WalkAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> StrafeAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess = "true"))
	EMovementType MovementType;

	// 아래 Speed들은 축에 따른 이동 속도가 아닌, 해당 상태일 때 속도를 여러 개로 지정한 것입니다.
	// X == Sprint, Y == Run, Z == Walk
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector WalkSpeeds = FVector(200.f, 175.f, 150.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector RunSpeeds = FVector(500.f, 350.f, 300.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector SprintSpeeds = FVector(700.f, 550.f, 500.f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurve; 

	bool bWantsToSprint = false;
	bool bWantsToWalk = false;

	// true일 때 캐릭터가 카메라 방향을 바라봅니다.
	bool bWantsToStrafe = true;
};
