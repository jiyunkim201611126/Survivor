// KJY

#include "MonsterMoveProcessor.h"

#include "MassExecutionContext.h"
#include "MassMonsterFragments.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"

UMonsterMoveProcessor::UMonsterMoveProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	// 멀티스레드 실행 가능하도록 설정합니다.
	bRequiresGameThreadExecution = false;
	
	ExecutionOrder.ExecuteInGroup = FName("Movement");
}

void UMonsterMoveProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FMonsterTransformFragment>(EMassFragmentAccess::ReadWrite);
}

void UMonsterMoveProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		const int32 NumEntities = Context.GetNumEntities();
		auto Transforms = Context.GetMutableFragmentView<FMonsterTransformFragment>();

		UPawnManagerSubsystem* PawnManager = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>();
		const APawn* PlayerPawn = PawnManager->GetAllPlayerPawns()[0];

		if (!PlayerPawn)
		{
			return;
		}

		const FVector PlayerLocation = PlayerPawn->GetActorLocation();
		
		const float DeltaTime = Context.GetDeltaTimeSeconds();
		
		UE_LOG(LogTemp, Warning, TEXT("Monster 0 Location: %s"), *Transforms[0].Transform.GetLocation().ToString());

		for (int32 Index = 0; Index < NumEntities; ++Index)
		{
			// 플레이어쪽을 바라본 채로 다가가도록 합니다.
			FVector CurrentLocation = Transforms[Index].Transform.GetLocation();

			FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();

			FVector NewVelocity = Direction * 300.f;

			FVector NewLocation = CurrentLocation + (NewVelocity * DeltaTime);
			Transforms[Index].Transform.SetLocation(NewLocation);

			Transforms[Index].Transform.SetRotation(Direction.Rotation().Quaternion());

		}
	});
}
