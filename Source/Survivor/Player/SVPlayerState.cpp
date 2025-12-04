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

	DOREPLIFETIME(ASVPlayerState, PlayerLevel);
	DOREPLIFETIME(ASVPlayerState, XP);
}

UAbilitySystemComponent* ASVPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ASVPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

int32 ASVPlayerState::GetPlayerLevel() const
{
	return PlayerLevel;
}

int32 ASVPlayerState::GetXP() const
{
	return XP;
}

void ASVPlayerState::SetLevel(int32 InLevel)
{
	PlayerLevel = InLevel;
	OnLevelChangedDelegate.Broadcast(PlayerLevel, false);
}

void ASVPlayerState::SetXP(const int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ASVPlayerState::AddToLevel(int32 InLevel)
{
	PlayerLevel += InLevel;
	OnLevelChangedDelegate.Broadcast(PlayerLevel, true);
}

void ASVPlayerState::AddToXP(const int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ASVPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(PlayerLevel, OldLevel != 0);
}

void ASVPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}
