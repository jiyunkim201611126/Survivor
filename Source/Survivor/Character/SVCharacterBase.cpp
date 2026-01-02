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

void ASVCharacterBase::ApplyKnockback(const FVector& KnockbackForce)
{
	LaunchCharacter(KnockbackForce, true, true);
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
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	// Collision의 충돌 주도권을 제거합니다.
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		MeshComponent->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetAllBodiesSimulatePhysics(true);
		MeshComponent->WakeAllRigidBodies();
		MeshComponent->KinematicBonesUpdateType = EKinematicBonesUpdateToPhysics::Type::SkipAllBones;

		MeshComponent->AddImpulse(FVector(100.f, 0.f, 0.f), NAME_None, true);
	}

	GetAbilitySystemComponent()->AddLooseGameplayTag(FSVGameplayTags::Get().CharacterState_Dead);
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

UAbilitySystemComponent* ASVCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
