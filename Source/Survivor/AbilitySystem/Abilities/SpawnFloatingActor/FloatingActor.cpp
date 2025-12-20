// KJY

#include "FloatingActor.h"

#include "Components/SphereComponent.h"
#include "Survivor/Survivor.h"
#include "Survivor/Interface/CombatInterface.h"

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

void AFloatingActor::Activate()
{
	// 활성화된 동안엔 Tick 함수가 돌도록 설정합니다.
	SetActorTickEnabled(true);

	// 타이머를 통해 Overlap되어 캐싱된 Actor들을 대상으로 OwnerAbility가 걸어둔 콜백을 호출합니다.
	if (GetWorld())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]()
		{
			for (const auto& OverlappedActor : OverlappedActors)
			{
				TArray<AActor*> ValidTargetActors;
				if (OverlappedActor.IsValid())
				{
					ValidTargetActors.Emplace(OverlappedActor.Get());
				}
				
				if (OnFloatingActorActivateDamageDelegate.IsBound())
				{
					OnFloatingActorActivateDamageDelegate.Execute(ValidTargetActors);
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
	OnFloatingActorActivateDamageDelegate.Unbind();
	OnLifeEndDelegate.Unbind();
	
	Super::EndPlay(EndPlayReason);
}

void AFloatingActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 자신과 Overlap된 Actor를 캐싱합니다.
	// Collision 설정에서 Enemy 외엔 전부 Ignore로 설정해뒀기 때문에 조건 검사를 간편하게 합니다.
	if (OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		OverlappedActors.Emplace(OtherActor);
	}
}

void AFloatingActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	// Overlap이 종료된 Actor를 제외합니다.
	OverlappedActors.RemoveSingleSwap(OtherActor);
}
