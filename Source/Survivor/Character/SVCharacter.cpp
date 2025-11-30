// KJY

#include "SVCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASVCharacter::ASVCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
}

void ASVCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASVCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(NewController)->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void ASVCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovement();
	UpdateRotation();
}

void ASVCharacter::UpdateRotation()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	
	if (bWantsToStrafe)
	{
		MovementComponent->bUseControllerDesiredRotation = true;
		MovementComponent->bOrientRotationToMovement = false;
	}
	else
	{
		MovementComponent->bUseControllerDesiredRotation = false;
		MovementComponent->bOrientRotationToMovement = true;
	}

	if (MovementComponent->IsFalling())
	{
		MovementComponent->RotationRate = FRotator(0.f, 0.f, 200.f);
	}
	else
	{
		MovementComponent->RotationRate = FRotator(0.f, 0.f, -1.f);
	}
}

void ASVCharacter::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = InputActionValue.Get<FVector2D>();
	const float ControlYaw = GetControlRotation().Yaw;

	const FVector ControlRightVector = UKismetMathLibrary::GetRightVector(FRotator(0.f, 0.f, ControlYaw));
	const FVector ControlForwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0.f, 0.f, ControlYaw));

	AddMovementInput(ControlRightVector, InputValue.X);
	AddMovementInput(ControlForwardVector, InputValue.Y);
}

void ASVCharacter::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = InputActionValue.Get<FVector2D>();

	AddControllerYawInput(InputValue.X);
	AddControllerPitchInput(InputValue.Y);
}

void ASVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
}
