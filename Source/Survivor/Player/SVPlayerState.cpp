// KJY

#include "SVPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/AbilitySystem/SVAttributeSet.h"

ASVPlayerState::ASVPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<USVAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<USVAttributeSet>("AttributeSet");

	SetNetUpdateFrequency(100.f);
}

void ASVPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASVPlayerState, Level);
	DOREPLIFETIME(ASVPlayerState, XP);
}

UAbilitySystemComponent* ASVPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASVPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
}

void ASVPlayerState::SetXP(const int32 InXP)
{
	XP = InXP;
}

void ASVPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
}

void ASVPlayerState::AddToXP(const int32 InXP)
{
	XP += InXP;
}

void ASVPlayerState::OnRep_Level(int32 OldLevel)
{
}

void ASVPlayerState::OnRep_XP(int32 OldXP)
{
}
