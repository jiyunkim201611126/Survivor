// KJY

#include "SVEnemy.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Component/GASManagerComponent/EnemyGASManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/SVAttributeSet.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"
#include "Survivor/Manager/SVGameplayTags.h"

ASVEnemy::ASVEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.5f);

	GetCapsuleComponent()->SetCollisionProfileName("Enemy");
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GASManagerComponent = CreateDefaultSubobject<UEnemyGASManagerComponent>(TEXT("GASManagerComponent"));
	
	AbilitySystemComponent = CreateDefaultSubobject<USVAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USVAttributeSet>(TEXT("AttributeSet"));

	GetCharacterMovement()->MaxWalkSpeed = 350.f;
}

void ASVEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	GASManagerComponent->AddCharacterStartupAbilities();

	// Enemy 객체는 풀링되므로 BeginPlay에선 RegisterPawn을 호출하지 않습니다.
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

	if (!GetAbilitySystemComponent() || GetAbilitySystemComponent()->HasMatchingGameplayTag(FSVGameplayTags::Get().CharacterState_Knockback))
	{
		return;
	}

	if (!MoveTargetActor.IsValid())
	{
		UpdateNearestTarget();
	}

	// 가까운 PlayerCharacter를 향해 다가갑니다.
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->MoveToActor(MoveTargetActor.Get(), -1, false);
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

void ASVEnemy::OnSpawnFromPool()
{
	Cast<UEnemyGASManagerComponent>(GASManagerComponent)->OnOwnerSpawnFromPool();
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

UAbilitySystemComponent* ASVEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
