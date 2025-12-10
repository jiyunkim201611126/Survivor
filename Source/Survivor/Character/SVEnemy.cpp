// KJY

#include "SVEnemy.h"

#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/CombatComponent/EnemyCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/AttributeSet/SVAttributeSet.h"

ASVEnemy::ASVEnemy()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("CombatComponent");

	AbilitySystemComponent = CreateDefaultSubobject<USVAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USVAttributeSet>("AttributeSet");
}

void ASVEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASVEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	CombatComponent->AddCharacterStartupAbilities();
	BP_PlaySpawnAnimation();
}

void ASVEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}

	SVAIController = Cast<ASVAIController>(NewController);
	SVAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	SVAIController->RunBehaviorTree(BehaviorTree);
}

void ASVEnemy::InitAbilityActorInfo() const
{
	CombatComponent->SetAbilitySystemComponent(AbilitySystemComponent);
	CombatComponent->SetAttributeSet(AttributeSet);
	CombatComponent->InitAbilityActorInfo();
}

void ASVEnemy::StartHide_Implementation()
{
	//GetMesh()->SetVisibility(false);
}

void ASVEnemy::EndHide_Implementation()
{
	//GetMesh()->SetVisibility(true);
}

UAbilitySystemComponent* ASVEnemy::GetAbilitySystemComponent() const
{
	return CombatComponent->GetAbilitySystemComponent();
}
