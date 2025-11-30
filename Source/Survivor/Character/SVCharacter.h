// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SVCharacter.generated.h"

class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
struct FInputActionValue;

UENUM(BlueprintType)
enum class EMovementType : uint8
{
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint"),
};

UCLASS()
class SURVIVOR_API ASVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASVCharacter();
	
	//~ Begin AActor Interface
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	//~ End of AActor Interface

	//~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	//~ End of APawn Interface

	//~ Begin ACharacter Interface
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ End of ACharacter Interface
	
private:
	void UpdateMovement();
	void UpdateRotation();
	
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	virtual void Jump() override;
	void Sprint(const FInputActionValue& InputActionValue);
	void Strafe();
	void Walk();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
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
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> StrafeAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	// 아래 Speed들은 축에 따른 이동 속도가 아닌, 해당 상태일 때 속도를 여러 개로 지정한 것입니다.
	// X == Sprint, Y == Run, Z == Walk
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector WalkSpeeds = FVector(200.f, 175.f, 150.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector RunSpeeds = FVector(500.f, 350.f, 300.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector SprintSpeeds = FVector(700.f, 700.f, 700.f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurve; 

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToSprint = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToWalk = false;

	// true일 때 캐릭터가 카메라 방향을 바라봅니다.
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToStrafe = true;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EMovementType MovementType;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	bool bJustLanded = false;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector LandVelocity;
};
