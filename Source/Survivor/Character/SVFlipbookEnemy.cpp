// KJY

#include "SVFlipbookEnemy.h"

#include "PaperFlipbookComponent.h"
#include "SVCharacter.h"
#include "Camera/CameraComponent.h"
#include "Survivor/Manager/PawnManagerSubsystem.h"

ASVFlipbookEnemy::ASVFlipbookEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(MeshComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.05f);
	
	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);
}

void ASVFlipbookEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CameraComponent.IsValid())
	{
		SetLocalCameraComponent();
		return;
	}

	UpdateFlipbookImage();
	UpdateFlipbookComponentDirection();
}

void ASVFlipbookEnemy::StartHide_Implementation()
{
	PaperFlipbookComponent->SetHiddenInGame(true);
}

void ASVFlipbookEnemy::EndHide_Implementation()
{
	PaperFlipbookComponent->SetHiddenInGame(false);
}

void ASVFlipbookEnemy::OnSpawnFromPool()
{
	Super::OnSpawnFromPool();

	// Pool에서 나온 직후 방향이 맞지 않아 '드득'하는 현상을 없애기 위해 Flipbook을 업데이트합니다.
	if (!CameraComponent.IsValid())
	{
		SetLocalCameraComponent();
		return;
	}
	
	UpdateFlipbookImage();
	UpdateFlipbookComponentDirection();
}

void ASVFlipbookEnemy::SetLocalCameraComponent()
{
	// 플레이어의 카메라를 가져와 캐싱합니다.
	if (UPawnManagerSubsystem* PawnManager = GetWorld()->GetGameInstance()->GetSubsystem<UPawnManagerSubsystem>())
	{
		for (const APawn* PlayerPawn : PawnManager->GetAllPlayerPawns())
		{
			if (PlayerPawn->IsLocallyControlled())
			{
				CameraComponent = PlayerPawn->FindComponentByClass<UCameraComponent>();
				break;
			}
		}
	}
}

void ASVFlipbookEnemy::UpdateFlipbookImage()
{
	// 카메라와 캐릭터의 정면 방향을 가져와 XY 평면에서 정규화합니다.
	const FVector CameraForward = CameraComponent->GetForwardVector().GetSafeNormal2D();
	const FVector CharacterForward = GetActorForwardVector().GetSafeNormal2D();

	// 내적을 계산합니다.
	const float Dot = FVector::DotProduct(CharacterForward, CameraForward);
	if (Dot < -0.5f)
	{
		// 내적이 -0.5보다 작으면 다른 방향을 보고 있으므로, 앞모습을 보여줍니다.
		CurrentDirection = EEnemyDirection::Front;
	}
	else if (Dot > 0.5f)
	{
		// 내적이 0.5보다 크면 카메라와 캐릭터가 같은 방향을 보고 있으므로, 뒷모습을 보여줍니다.
		CurrentDirection = EEnemyDirection::Back;
	}
	else
	{
		// 내적이 -0.5이상, 0.5이하인 경우, 즉 옆모습을 보여줘야 하는 경우 들어오는 분기입니다.
		// 외적의 Z값을 통해 좌우를 결정합니다.
		const FVector Cross = FVector::CrossProduct(CameraForward, CharacterForward);
		CurrentDirection = Cross.Z > 0.f ? EEnemyDirection::Right : EEnemyDirection::Left;
	}

	UPaperFlipbook* CurrentFlipbook = Flipbooks.FindRef(CurrentDirection);
	if (CurrentFlipbook)
	{
		PaperFlipbookComponent->SetFlipbook(CurrentFlipbook);
	}
}

void ASVFlipbookEnemy::UpdateFlipbookComponentDirection() const
{
	FRotator LookAtRotation = CameraComponent->GetRightVector().Rotation();
	LookAtRotation.Roll = CameraComponent->GetComponentRotation().Pitch;
	
	PaperFlipbookComponent->SetWorldRotation(LookAtRotation);
}
