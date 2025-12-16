// KJY

#include "PlayerGASManagerComponent.h"

#include "Survivor/AbilitySystem/SVAbilitySystemComponent.h"
#include "Survivor/Player/SVPlayerController.h"
#include "Survivor/Player/SVPlayerState.h"
#include "Survivor/UI/HUD/SVHUD.h"

void UPlayerGASManagerComponent::InitAbilityActorInfo()
{
	ASVPlayerState* SVPlayerState = GetPlayerState<ASVPlayerState>();
	check(SVPlayerState);

	AbilitySystemComponent = SVPlayerState->GetAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(SVPlayerState, GetPawn<APawn>());
	Cast<USVAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = SVPlayerState->GetAttributeSet();

	ApplyEffectToSelf(DefaultAttributes, SVPlayerState->GetPlayerLevel());

	if (ASVPlayerController* SVPlayerController = GetController<ASVPlayerController>())
	{
		if (ASVHUD* SVHUD = Cast<ASVHUD>(SVPlayerController->GetHUD()))
		{
			SVHUD->InitHUD(SVPlayerController, SVPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void UPlayerGASManagerComponent::AddCharacterStartupAbilities() const
{
	if (!HasAuthority())
	{
		return;
	}

	// ASC 가져와서 부여 함수 호출
	USVAbilitySystemComponent* SVASC = CastChecked<USVAbilitySystemComponent>(AbilitySystemComponent);
	SVASC->AddCharacterAbilitiesWithActive(StartupAbilities);
}
