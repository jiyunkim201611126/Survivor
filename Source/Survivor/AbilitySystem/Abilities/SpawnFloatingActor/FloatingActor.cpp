// KJY

#include "FloatingActor.h"

#include "Components/SphereComponent.h"
#include "Survivor/Survivor.h"
#include "Survivor/Interface/CombatInterface.h"
#include "Survivor/Manager/ObjectPoolManagerSubsystem.h"

AFloatingActor::AFloatingActor(const FObjectInitializer& ObjectInitializer)
	// 기본적으로 이 클래스는 Collision을 SphereComponent로 설정하지만, 하위 C++ 클래스에서 변경 가능하도록 만드는 코드입니다.
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USphereComponent>(TEXT("CollisionComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComponent = ObjectInitializer.CreateDefaultSubobject<UShapeComponent>(this, TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
}

void AFloatingActor::SetLifeTime(const float InLifeTime)
{
	// 지정된 시간 이후 콜백을 호출, OwnerAbility가 이 Actor를 Pool에 반환합니다.
	if (GetWorld())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			SetActorTickEnabled(false);
			
			TargetLocation = UObjectPoolManagerSubsystem::PoolLocation;
			TargetRotation = FRotator::ZeroRotator;
			
			GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
			
			if (OnLifeEndDelegate.IsBound())
			{
				OnLifeEndDelegate.Execute(this);
			}
		});
		
		GetWorld()->GetTimerManager().SetTimer(
			LifeTimerHandle,
			TimerDelegate,
			InLifeTime,
			false,
			InLifeTime);
	}
}

void AFloatingActor::Activate(const FVector& InActivateLocation, const FRotator& InActivateRotation)
{
	// Activate 시점에 Ability가 정해준 위치 정보를 캐싱합니다.
	TargetLocation = InActivateLocation;
	TargetRotation = InActivateRotation;
	OnActivate();
	
	// 활성화된 동안엔 Tick 함수가 돌도록 설정합니다.
	SetActorTickEnabled(true);

	// 타이머를 통해 주기적으로 Overlap되어 캐싱된 Actor들을 대상으로 OwnerAbility가 걸어둔 콜백을 호출합니다.
	if (GetWorld())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			// 데미지로 인해 TargetActor가 사망하게 될 경우, Collision이 꺼지면서 OverlappedCombatActors의 개수가 달라져 위험합니다.
			// 따라서 현재 Overlap된 Actor를 이 스코프 안에서 캐싱한 뒤 사용합니다.
			TArray<AActor*> ValidActors;
			for (const TWeakObjectPtr<AActor>& TargetActor : OverlappedCombatActors)
			{
				if (TargetActor.IsValid())
				{
					ValidActors.Emplace(TargetActor.Get());
				}
			}
			
			// 현재 살아있는 Actor들에게 Effect를 적용합니다.
			if (OnFloatingActorActivateDelegate.IsBound())
			{
				for (AActor* TargetActor : ValidActors)
				{
					OnFloatingActorActivateDelegate.Execute(TargetActor);
					UE_LOG(LogTemp, Log, TEXT("AFloatingActor::Activate - Damage Applied"));
				}
			}
		});
		
		GetWorld()->GetTimerManager().SetTimer(
			DamageTimerHandle,
			TimerDelegate,
			DamageFrequency,
			true,
			0);
	}
}

void AFloatingActor::ResetOverlapActors()
{
	FirstOverlapThisActive.Reset();
	OverlappedCombatActors.Reset();
}

void AFloatingActor::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

void AFloatingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetActorTickEnabled(false);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
	}
	
	CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnComponentBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnComponentEndOverlap);
	OnFloatingActorActivateDelegate.Unbind();
	OnLifeEndDelegate.Unbind();
	
	Super::EndPlay(EndPlayReason);
}

void AFloatingActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 자신과 Overlap된 Actor를 캐싱합니다.
	// Collision 설정에서 Enemy 외엔 전부 Ignore로 설정해뒀기 때문에 조건 검사를 간편하게 합니다.
	if (OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		OverlappedCombatActors.Emplace(OtherActor);
		
		if (OnFloatingActorActivateDelegate.IsBound())
		{
			// Active 후 처음 Overlap된 대상에겐 Overlap 즉시 Effect를 한 번 적용합니다.
			if (!FirstOverlapThisActive.Contains(OtherActor))
			{
				FirstOverlapThisActive.Emplace(OtherActor);
				OnFloatingActorActivateDelegate.Execute(OtherActor);
			}
		}
	}
}

void AFloatingActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	// Overlap이 종료된 Actor를 제외합니다.
	if (OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		OverlappedCombatActors.RemoveSingleSwap(OtherActor);
	}
}
