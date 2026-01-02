// KJY
/*
#include "SVFlipbookEnemy_NotUse_PawnBase.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/GASManagerComponent/EnemyGASManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/SVAttributeSet.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"
#include "Survivor/Manager/SVGameplayTags.h"

ASVFlipbookEnemy_NotUse_PawnBase::ASVFlipbookEnemy_NotUse_PawnBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.SetTickFunctionEnable(false);
	SetActorTickInterval(0.05f);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetCollisionProfileName("Enemy");
	CapsuleComponent->SetGenerateOverlapEvents(true);
	
	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);
	PaperFlipbookComponent->SetGenerateOverlapEvents(false);
	PaperFlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->MaxSpeed = 300.f;
	MovementComponent->Deceleration = 1000.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GASManagerComponent = CreateDefaultSubobject<UEnemyGASManagerComponent>(TEXT("GASManagerComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<USVAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<USVAttributeSet>(TEXT("AttributeSet"));
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* ASVFlipbookEnemy_NotUse_PawnBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCapsuleComponent* ASVFlipbookEnemy_NotUse_PawnBase::GetCombatCapsuleComponent() const
{
	return CapsuleComponent;
}

void ASVFlipbookEnemy_NotUse_PawnBase::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	GASManagerComponent->AddCharacterStartupAbilities();

	// Enemy 객체는 풀링되므로 BeginPlay에선 RegisterPawn을 호출하지 않습니다.
}

void ASVFlipbookEnemy_NotUse_PawnBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->UnRegisterAIPawn(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ASVFlipbookEnemy_NotUse_PawnBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CameraComponent.IsValid())
	{
		SetLocalCameraComponent();
	}
	UpdateFlipbookImage();
	UpdateFlipbookComponentDirection();

	ApplyGravity(DeltaSeconds);

	// 넉백 중인 경우 Controller가 움직임에 개입하지 못 하도록 합니다.
	if (!AbilitySystemComponent || AbilitySystemComponent->HasMatchingGameplayTag(FSVGameplayTags::Get().CharacterState_Knockback))
	{
		return;
	}

	UpdateMovement();
}

void ASVFlipbookEnemy_NotUse_PawnBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		if (UPawnManagerSubsystem* PawnManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>())
		{
			PawnManagerSubsystem->RegisterAIPawn(this);
		}
	}
}

void ASVFlipbookEnemy_NotUse_PawnBase::UnPossessed()
{
	if (HasAuthority())
	{
		if (UPawnManagerSubsystem* PawnManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>())
		{
			PawnManagerSubsystem->UnRegisterAIPawn(this);
		}
	}
	
	Super::UnPossessed();
}

void ASVFlipbookEnemy_NotUse_PawnBase::ApplyGravity(const float DeltaTime)
{
	// 중력 적용 로직을 시작합니다.
	FHitResult GroundHit;
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation - FVector(0.f, 0.f, 10000.f);
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(GroundHit, StartLocation, EndLocation, ECC_WorldStatic, Params);
	if (bHit)
	{
		// 검출에 성공한 경우, 계산에 필요한 값을 가져옵니다.
		const float GroundZ = GroundHit.ImpactPoint.Z;
		const float DistanceToGround = GroundHit.Distance;
		const float HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();

		// CapsuleComponent의 높이 절반보다 긴 거리에서 라인트레이스가 검출되면 공중에 떠있는 상태라고 간주합니다.
		// 약간의 오차를 통해 Actor가 떨리는 현상을 방지합니다.
		if (DistanceToGround > HalfHeight + 1.f)
		{
			VerticalVelocity -= GravityForce * DeltaTime;

			FVector NewLocation = GetActorLocation();
			NewLocation.Z += VerticalVelocity * DeltaTime;

			if (NewLocation.Z < GroundZ + HalfHeight)
			{
				NewLocation.Z = GroundZ + HalfHeight;
				VerticalVelocity = 0.f;
			}

			SetActorLocation(NewLocation);
		}
	}
}

void ASVFlipbookEnemy_NotUse_PawnBase::UpdateMovement() const
{
	UPawnManagerSubsystem* PawnManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!PawnManagerSubsystem || !AIController)
	{
		return;
	}

	// 가장 가까운 PlayerPawn의 NavMesh 위치를 반환받아 그 위치로 이동합니다.
	const FVector NewLocation = PawnManagerSubsystem->GetNearestPlayerPawnNavLocation(GetActorLocation());
	if (!NewLocation.IsNearlyZero())
	{
		AIController->MoveToLocation(NewLocation);
	}
}

void ASVFlipbookEnemy_NotUse_PawnBase::ApplyKnockback(const FVector& KnockbackImpulse)
{
	// 기존 움직임을 중단하고 넉백 방향으로 Velocity를 할당합니다.
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();

		MovementComponent->Velocity = FVector(KnockbackImpulse.X, KnockbackImpulse.Y, 0.f);
		VerticalVelocity = KnockbackImpulse.Z;
	}
}

void ASVFlipbookEnemy_NotUse_PawnBase::Die()
{
}

void ASVFlipbookEnemy_NotUse_PawnBase::OnSpawnFromPool()
{
	GASManagerComponent->OnOwnerSpawnFromPool();

	// Tick은 다음 프레임부터 호출되기 때문에, 여기서 한 번 업데이트해줍니다.
	if (!CameraComponent.IsValid())
	{
		SetLocalCameraComponent();
	}
	UpdateFlipbookImage();
	UpdateFlipbookComponentDirection();

	SetActorTickEnabled(true);
}

void ASVFlipbookEnemy_NotUse_PawnBase::InitAbilityActorInfo() const
{
	GASManagerComponent->SetAbilitySystemComponent(AbilitySystemComponent);
	GASManagerComponent->SetAttributeSet(AttributeSet);
	GASManagerComponent->InitAbilityActorInfo();
}

void ASVFlipbookEnemy_NotUse_PawnBase::SetLocalCameraComponent()
{
	// 플레이어의 카메라를 가져와 캐싱합니다.
	if (UPawnManagerSubsystem* PawnManager = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>())
	{
		for (const APawn* PlayerPawn : PawnManager->GetAllPlayerPawns())
		{
			if (PlayerPawn->IsLocallyControlled())
			{
				CameraComponent = PlayerPawn->FindComponentByClass<UCameraComponent>();
				break;
			}
		}
	}
}

void ASVFlipbookEnemy_NotUse_PawnBase::UpdateFlipbookImage() const
{
	// 카메라와 캐릭터의 정면 방향을 가져와 XY 평면에서 정규화합니다.
	const FVector CameraForward = CameraComponent->GetForwardVector().GetSafeNormal2D();
	const FVector CharacterForward = GetActorForwardVector().GetSafeNormal2D();

	// 내적을 계산합니다.
	const float Dot = FVector::DotProduct(CharacterForward, CameraForward);
	
	EFlipbookDirection CurrentDirection;
	if (Dot < -0.5f)
	{
		// 내적이 -0.5보다 작으면 다른 방향을 보고 있으므로, 앞모습을 보여줍니다.
		CurrentDirection = EFlipbookDirection::Front;
	}
	else if (Dot > 0.5f)
	{
		// 내적이 0.5보다 크면 카메라와 캐릭터가 같은 방향을 보고 있으므로, 뒷모습을 보여줍니다.
		CurrentDirection = EFlipbookDirection::Back;
	}
	else
	{
		// 내적이 -0.5이상, 0.5이하인 경우, 즉 옆모습을 보여줘야 하는 경우 들어오는 분기입니다.
		// 외적의 Z값을 통해 좌우를 결정합니다.
		// 언리얼은 왼손 좌표계를 사용하기 때문에 Z가 양수면 캐릭터가 카메라 기준으로 오른쪽을 바라보고 있습니다.
		const FVector Cross = FVector::CrossProduct(CameraForward, CharacterForward);
		CurrentDirection = Cross.Z > 0.f ? EFlipbookDirection::Right : EFlipbookDirection::Left;
	}

	UPaperFlipbook* CurrentFlipbook = Flipbooks.FindRef(CurrentDirection);
	if (CurrentFlipbook)
	{
		PaperFlipbookComponent->SetFlipbook(CurrentFlipbook);
	}
}

void ASVFlipbookEnemy_NotUse_PawnBase::UpdateFlipbookComponentDirection() const
{
	FRotator LookAtRotation = CameraComponent->GetRightVector().Rotation();
	LookAtRotation.Roll = CameraComponent->GetComponentRotation().Pitch;
	
	PaperFlipbookComponent->SetWorldRotation(LookAtRotation);
}
*/