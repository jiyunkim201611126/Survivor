// KJY

#include "SVCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Component/GASManagerComponent/GASManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Survivor/Manager/SVGameplayTags.h"

ASVCharacterBase::ASVCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* ASVCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCapsuleComponent* ASVCharacterBase::GetCombatCapsuleComponent() const
{
	return GetCapsuleComponent();
}

void ASVCharacterBase::InitAbilityActorInfo() const
{
	GASManagerComponent->SetAbilitySystemComponent(AbilitySystemComponent);
	GASManagerComponent->SetAttributeSet(AttributeSet);
	GASManagerComponent->InitAbilityActorInfo();
}

void ASVCharacterBase::ApplyKnockback(const FVector& KnockbackForce)
{
	const bool bDead = GetAbilitySystemComponent()->HasMatchingGameplayTag(FSVGameplayTags::Get().CharacterState_Dead);
	if (bDead)
	{
		if (GetMesh())
		{
			GetMesh()->AddImpulseToAllBodiesBelow(KnockbackForce, NAME_None, true, true);
		}
	}
	else
	{
		LaunchCharacter(KnockbackForce, true, true);
	}
}

void ASVCharacterBase::Die()
{
	UnregisterPawn(this);

	MulticastDeath();
}

void ASVCharacterBase::MulticastDeath_Implementation()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	// Collision의 충돌 주도권을 제거합니다.
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetAbilitySystemComponent()->AddLooseGameplayTag(FSVGameplayTags::Get().CharacterState_Dead);

	StartOnDeathTimer();
}

void ASVCharacterBase::StartOnDeathTimer()
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]()
	{
		OnDeath();
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.f, false, 2.f);
}

void ASVCharacterBase::OnDeath()
{
}
