// KJY

#include "SVEnemy.h"

#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/GASManagerComponent/EnemyGASManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/SVAttributeSet.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"

ASVEnemy::ASVEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	
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
}

void ASVEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	GASManagerComponent->AddCharacterStartupAbilities();
	BP_PlaySpawnAnimation();

	if (HasAuthority())
	{
		GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>()->RegisterAIPawn(this);
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

void ASVEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		SVAIController = Cast<ASVAIController>(NewController);
		SVAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		SVAIController->RunBehaviorTree(BehaviorTree);

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

void ASVEnemy::ApplyKnockback(const FVector& KnockbackForce)
{
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
