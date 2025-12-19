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
	SetActorTickEnabled(true);
	
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
	if (OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		OverlappedActors.Emplace(OtherActor);
	}
}

void AFloatingActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	OverlappedActors.RemoveSingleSwap(OtherActor);
}
