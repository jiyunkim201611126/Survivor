// KJY

#include "PawnManagerSubsystem.h"

void UPawnManagerSubsystem::RegisterPlayerPawn(APawn* InPawn)
{
	if (!PlayerPawns.Contains(InPawn))
	{
		PlayerPawns.Add(InPawn);
		bPlayerPawnsCacheDirty = true;
	}
}

void UPawnManagerSubsystem::UnRegisterPlayerPawn(APawn* InPawn)
{
	PlayerPawns.RemoveSingleSwap(InPawn);
	CachedPlayerPawns.RemoveSingleSwap(InPawn);
	bPlayerPawnsCacheDirty = true;
}

TArray<APawn*> UPawnManagerSubsystem::GetAllPlayerPawns()
{
	if (bPlayerPawnsCacheDirty)
	{
		CachedPlayerPawns.Reset();
		for (const TWeakObjectPtr<APawn>& PlayerPawn : PlayerPawns)
		{
			if (PlayerPawn.IsValid())
			{
				CachedPlayerPawns.Add(PlayerPawn.Get());
			}
		}
		bPlayerPawnsCacheDirty = false;
	}
	return CachedPlayerPawns;
}

void UPawnManagerSubsystem::RegisterAIPawn(APawn* InPawn)
{
	if (!AIPawns.Contains(InPawn))
	{
		AIPawns.Add(InPawn);
		bAIPawnsCacheDirty = true;
	}
}

void UPawnManagerSubsystem::UnRegisterAIPawn(APawn* InPawn)
{
	AIPawns.RemoveSingleSwap(InPawn);
	CachedAIPawns.RemoveSingleSwap(InPawn);
	bAIPawnsCacheDirty = true;
}

TArray<APawn*> UPawnManagerSubsystem::GetAllAIPawns()
{
	if (bAIPawnsCacheDirty)
	{
		CachedAIPawns.Reset();
		for (const TWeakObjectPtr<APawn>& AIPawn : AIPawns)
		{
			if (AIPawn.IsValid())
			{
				CachedAIPawns.Add(AIPawn.Get());
			}
		}
		bAIPawnsCacheDirty = false;
	}
	return CachedAIPawns;
}

APawn* UPawnManagerSubsystem::GetRandomPlayerPawn()
{
	// Refresh를 위해 아래 함수를 한 번 호출한 뒤 진행합니다.
	GetAllPlayerPawns();
	if (PlayerPawns.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, PlayerPawns.Num() - 1);
		return CachedPlayerPawns[RandomIndex];
	}

	return nullptr;
}

APawn* UPawnManagerSubsystem::GetLocalPlayerPawn()
{
	GetAllPlayerPawns();
	for (APawn* PlayerPawn : CachedPlayerPawns)
	{
		if (PlayerPawn->IsLocallyControlled())
		{
			return PlayerPawn;
		}
	}

	return nullptr;
}