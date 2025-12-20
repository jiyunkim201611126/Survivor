// KJY

#include "SVCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KismetAnimationLibrary.h"
#include "Component/GASManagerComponent/PlayerGASManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Survivor/Camera/SVCameraComponent.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"

ASVCharacter::ASVCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
	GetCapsuleComponent()->SetCollisionProfileName("Player");
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	CameraComponent = CreateDefaultSubobject<USVCameraComponent>("SVCameraComponent");
	CameraComponent->SetupAttachment(GetRootComponent());
	
	GASManagerComponent = CreateDefaultSubobject<UPlayerGASManagerComponent>(TEXT("GASManagerComponent"));
}

void ASVCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->RegisterPlayerPawn(this);
	}
}

void ASVCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->UnRegisterPlayerPawn(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ASVCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovement();
	UpdateRotation();
}

void ASVCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void ASVCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	GASManagerComponent->AddCharacterStartupAbilities();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(NewController)->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}

	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->RegisterPlayerPawn(this);
	}
}

void ASVCharacter::UnPossessed()
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->UnRegisterPlayerPawn(this);
	}
	
	Super::UnPossessed();
}

void ASVCharacter::UpdateMovement()
{
	// 달리기 입력이 들어온 경우 무조건 Run 상태로 설정합니다.
	// 아직 MM Sample 프로젝트에 Sprint 애니메이션이 없어 불가피합니다.
	if (bWantsToSprint)
	{
		MovementType = EMovementType::Run;
	}
	else
	{
		MovementType = bWantsToWalk ? EMovementType::Walk : EMovementType::Run;
	}

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	// 현재 이동 벡터(Velocity)가 캐릭터의 바라보는 방향 기준으로 몇 도인지 계산합니다.
	// 0도 = 정면, 90도 = 오른쪽, -90도 = 왼쪽, 180도 = 뒤
	const float Direction = UKismetAnimationLibrary::CalculateDirection(MovementComponent->Velocity, GetActorRotation());
	// 위 계산된 각도의 절대값으로 속도 보정 값을 Curve에서 가져옵니다.
	const float StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(FMath::Abs(Direction));

	// 현재 상태에 따라 Speed 집합을 선택합니다.
	FVector SelectedSpeeds;
	if (bWantsToSprint)
	{
		SelectedSpeeds = SprintSpeeds;
	}
	else
	{
		if (MovementType == EMovementType::Walk)
		{
			SelectedSpeeds = WalkSpeeds;
		}
		else // if (MovementType == EMovementType::Run)
		{
			SelectedSpeeds = RunSpeeds;
		}
	}

	// StrafeSpeedMap 값이 2에 가까울수록 느려지도록 보간합니다.
	// 즉, 캐릭터가 앞을 바라보고 이동해야 이동속도가 빠릅니다.
	float ResultSpeed;
	if (StrafeSpeedMap < 1.f)
	{
		ResultSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 0.f, 1.f, SelectedSpeeds.X, SelectedSpeeds.Y);
	}
	else
	{
		ResultSpeed = UKismetMathLibrary::MapRangeClamped(StrafeSpeedMap, 1.f, 2.f, SelectedSpeeds.Y, SelectedSpeeds.Z);
	}

	MovementComponent->MaxWalkSpeed = ResultSpeed;
	MovementComponent->MaxWalkSpeedCrouched = ResultSpeed;
}

void ASVCharacter::UpdateRotation() const
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	if (MovementComponent->IsFalling())
	{
		MovementComponent->RotationRate = FRotator(0.f, -1.f, 0.f);
	}
	else
	{
		MovementComponent->RotationRate = FRotator(0.f, 200.f, 0.f);
	}
}

void ASVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Sprint);
	EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ThisClass::Walk);
	EnhancedInputComponent->BindAction(StrafeAction, ETriggerEvent::Started, this, &ThisClass::Strafe);
}

void ASVCharacter::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = InputActionValue.Get<FVector2D>();
	const float ControlYaw = GetControlRotation().Yaw;

	const FVector ControlRightVector = UKismetMathLibrary::GetRightVector(FRotator(0.f, ControlYaw, 0.f));
	const FVector ControlForwardVector = UKismetMathLibrary::GetForwardVector(FRotator(0.f, ControlYaw, 0.f));

	AddMovementInput(ControlRightVector, InputValue.X);
	AddMovementInput(ControlForwardVector, InputValue.Y);
}

void ASVCharacter::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = InputActionValue.Get<FVector2D>();

	AddControllerYawInput(InputValue.X);
	AddControllerPitchInput(InputValue.Y);
}

void ASVCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		Super::Jump();
	}
}

void ASVCharacter::Sprint(const FInputActionValue& InputActionValue)
{
	const bool InputValue = InputActionValue.Get<bool>();
	bWantsToSprint = InputValue;
	bWantsToWalk = false;
}

void ASVCharacter::Walk()
{
	if (!bWantsToSprint)
	{
		bWantsToWalk = !bWantsToWalk;
	}
}

void ASVCharacter::Strafe()
{
	//bWantsToStrafe = !bWantsToStrafe;
	
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
}

UAbilitySystemComponent* ASVCharacter::GetAbilitySystemComponent() const
{
	return GASManagerComponent->GetAbilitySystemComponent();
}
