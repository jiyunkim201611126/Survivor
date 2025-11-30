// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SVCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SURVIVOR_API ASVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASVCharacter();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMovement();
	void UpdateRotation();
	
private:
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(BlueprintReadOnly)
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
	TObjectPtr<UInputAction> StrafeAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> WalkAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector WalkSpeeds = FVector(200.f, 175.f, 150.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector RunSpeeds = FVector(500.f, 350.f, 300.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector SprintSpeeds = FVector(700.f, 700.f, 700.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	FVector CrouchSpeeds = FVector(200.f, 175.f, 150.f);

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AnalogWalkRunThreshold = 0.7f;;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurve; 

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToSprint = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToWalk = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToStrafe = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bWantsToAim = false;
};
