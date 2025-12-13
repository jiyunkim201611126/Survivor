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
#include "Survivor/Manager/SVGameplayTags.h"

ASVEnemy::ASVEnemy()
{
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

void ASVEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASVEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	GASManagerComponent->AddCharacterStartupAbilities();
	BP_PlaySpawnAnimation();

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

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

void ASVEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 이 이벤트로 들어왔다면 OtherActor는 반드시 PlayerCharacter여야 합니다.
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	
	if (OtherActor && ASC->AbilityActorInfo.IsValid())
	{
		FScopedAbilityListLock AbilityListLock(*ASC);
		const FSVGameplayTags& GameplayTags = FSVGameplayTags::Get();

		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.Ability && Spec.Ability->GetAssetTags().HasTagExact(GameplayTags.EnemyAbilities_Normal))
			{
				FGameplayEventData EventData;
				EventData.Target = OtherActor;

				ASC->TriggerAbilityFromGameplayEvent(Spec.Handle, ASC->AbilityActorInfo.Get(), FGameplayTag(), &EventData, *ASC);
				return;
			}
		}
	}
}

void ASVEnemy::ApplyKnockback(const FVector& KnockbackForce)
{
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
	return GASManagerComponent->GetAbilitySystemComponent();
}
