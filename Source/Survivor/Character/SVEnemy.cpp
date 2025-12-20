// KJY

#include "SVEnemy.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Component/GASManagerComponent/EnemyGASManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/SVAttributeSet.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"
#include "Survivor/Manager/SVGameplayTags.h"

ASVEnemy::ASVEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);

	CapsuleComponent->SetCollisionProfileName("Enemy");
	CapsuleComponent->SetGenerateOverlapEvents(true);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GASManagerComponent = CreateDefaultSubobject<UEnemyGASManagerComponent>("CombatComponent");

	AbilitySystemComponent = CreateDefaultSubobject<USVAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USVAttributeSet>("AttributeSet");

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
	FloatingPawnMovement->MaxSpeed = 350.f;
}

void ASVEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	GASManagerComponent->AddCharacterStartupAbilities();
	BP_PlaySpawnAnimation();
	UpdateNearestTarget();
	
	if (HasAuthority())
	{
		UPawnManagerSubsystem* PawnManager = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
		PawnManager->RegisterAIPawn(this);
	}
}

void ASVEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->UnRegisterAIPawn(this);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ASVEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!MoveTargetActor.IsValid() || AbilitySystemComponent->HasMatchingGameplayTag(FSVGameplayTags::Get().CharacterState_Knockback))
	{
		UpdateNearestTarget();
	}

	// 플레이어를 향해 움직입니다.
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToActor(MoveTargetActor.Get());
	}
}

void ASVEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->RegisterAIPawn(this);
	}
}

void ASVEnemy::UnPossessed()
{
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->UnRegisterAIPawn(this);
	}
	
	Super::UnPossessed();
}

void ASVEnemy::InitAbilityActorInfo() const
{
	GASManagerComponent->SetAbilitySystemComponent(AbilitySystemComponent);
	GASManagerComponent->SetAttributeSet(AttributeSet);
	GASManagerComponent->InitAbilityActorInfo();
}

void ASVEnemy::UpdateNearestTarget()
{
	UPawnManagerSubsystem* PawnManager = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
	float ClosestDistanceSquared = FLT_MAX;
	APawn* ClosestPlayerPawn = nullptr;
	const FVector EnemyLocation = GetActorLocation();
	for (APawn* PlayerPawn : PawnManager->GetAllPlayerPawns())
	{
		const float DistanceSquared = FVector::DistSquared(PlayerPawn->GetActorLocation(), EnemyLocation);
		if (DistanceSquared < ClosestDistanceSquared)
		{
			ClosestDistanceSquared = DistanceSquared;
			ClosestPlayerPawn = PlayerPawn;
		}
	}
	MoveTargetActor = ClosestPlayerPawn;
}

void ASVEnemy::ApplyKnockback(const FVector& KnockbackForce)
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToLocation(GetActorLocation() + KnockbackForce);
	}
}

void ASVEnemy::StartHide_Implementation()
{
}

void ASVEnemy::EndHide_Implementation()
{
}

UAbilitySystemComponent* ASVEnemy::GetAbilitySystemComponent() const
{
	return GASManagerComponent->GetAbilitySystemComponent();
}

UCapsuleComponent* ASVEnemy::GetCapsuleComponent() const
{
	return CapsuleComponent;
}
