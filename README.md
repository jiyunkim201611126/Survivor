# Survivor

GAS 기반 뱀파이어 서바이버 라이크 프로젝트입니다.
수많은 적이 동시에 쫓아오고, 그 속에서 버티는 재미를 느끼는 장르인만큼 최적화면에서 기술적인 도전이 있었습니다.
- 엔진: 5.6
- IDE: Rider
- 인원: 1인개발
- 기간: 1개월

# 주요 기술

## Slate
장르 특성상 무수히 많은 DamageText를 렌더링해야 합니다. WidgetComponent를 사용하면 쉽고 빠른 구현이 가능하나, WidgetComponent를 소유한 Actor를 스폰해야 합니다. 움직임을 위해 ProjectileComponent도 필요할 수 있습니다. ObjectPool 패턴을 사용할 수 있겠지만, 그럼에도 여전히 Tick 혹은 물리적인 움직임 계산이 필요할 수 있습니다.

따라서 Slate를 사용했습니다.

<img width="339" height="248" alt="image" src="https://github.com/user-attachments/assets/cdfbcd9a-7a93-488e-851c-88a6fdd1eaf8" />

데미지가 발생한 장소와 데미지 값을 Subsystem에 넘겨 기록, 이를 SCompoundWidget을 상속받은 SDamageTextCanvas 클래스의 OnPaint에서 가져와 렌더링합니다. 애니메이션은 캐싱한 장소(Location)을 Subsystem이 지속적으로 업데이트하는 것으로 구현합니다.

```cpp
int32 SDamageTextCanvas::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 ResultLayerID = LayerId;

	const float DPI_Scale = AllottedGeometry.GetAccumulatedLayoutTransform().GetScale();
	if (!Manager.IsValid() || FMath::IsNearlyZero(DPI_Scale))
	{
		return ResultLayerID;
	}
	
	FSlateFontInfo FontInfo;
	if (Font)
	{
		FontInfo = FSlateFontInfo(Font, 24.f);
	}
	else
	{
		FontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
		FontInfo.Size = 24.f;
	}

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	for (const FDamageInfo& Info : *Manager->GetActiveDamageNumbers())
	{
		// 이번 Damage Text의 3D상의 위치가 실제 스크린에 그려지고 있는 경우만 들어가는 분기입니다.
		if (Info.bIsOnScreen)
		{
			// 그려질 텍스트의 크기를 미리 계산합니다.
			const FVector2D TextSize = FontMeasure->Measure(Info.DamageAmountText, FontInfo);

			// 원래 위치에서 텍스트 너비의 절반만큼 왼쪽으로 이동하여 중앙을 맞추고, 해상도에 맞게 다시 계산합니다.
			FVector2D DrawPosition = Info.ScreenPosition / DPI_Scale;
			DrawPosition.X -= TextSize.X * 0.5f;
			// 점점 위쪽으로 이동하는 애니메이션 추가합니다.
			DrawPosition.Y -= Info.VerticalOffset;

			// 남은 수명에 따라 투명도를 조정합니다.
			float Alpha = Info.LifeTime > 0.5f ? 1.f : Info.LifeTime / 0.5f;
			FLinearColor TextColor = FLinearColor(1.f, 0.8f, 0.2f, Alpha);

			FSlateDrawElement::MakeText(
				OutDrawElements,
				++ResultLayerID,
				AllottedGeometry.ToPaintGeometry(FVector2D(1,1), FSlateLayoutTransform(DrawPosition)),
				Info.DamageAmountText,
				FontInfo,
				ESlateDrawEffect::None,
				TextColor
				);
		}
	}

	return ResultLayerID;
}
```

100개의 DamageText가 있다면 DamageText 객체 100개가 스폰되어 각자 로직이 도는 것이 아닌, Canvas 하나가 100개의 DamageText를 한 번에 렌더링하게 됩니다.

## InstancedStaticMeshComponent(ISM) / Entity / 공간 분할 알고리즘

DamageText와 마찬가지로 적 또한 무수히 많이 렌더링해야 합니다. 300마리 이상의 적이 동시에 플레이어를 추격하는 일도 비일비재한 장르입니다. 따라서 ACharacter를 상속받은 클래스를 그대로 사용하는 데엔 무리가 있습니다. 언리얼의 MassEntity를 사용해보려 했으나, 베타 단계의 기술이기 때문에 버전마다 프레임워크가 상이하고 참고할 수 있는 자료도 적어 직접 구현하기로 했습니다.

<img width="1537" height="446" alt="image" src="https://github.com/user-attachments/assets/81130efb-ada2-461f-b6ee-9c05029bfadd" />

많은 적이 몰려오고 있지만, 이 모든 적을 Draw Call 1회로 렌더링하는 모습입니다. 우측을 보시면 EntitySpawner만 선택되어 있습니다.

EQS로 NavMesh 위치를 가져온 뒤 ISM으로 넘겨서 다수의 Mesh를 동시에 스폰하고, TickableWorldSubsystem에서 위치를 조정하다가 플레이어와 충분히 가까워지면 GAS 관련 Component를 가진 객체로 교체합니다. 플레이어에게 다가오는 과정은 NavMesh를 이용해 경로를 생성했으며, Collision 느낌을 구현하기 위해 Grid 공간분할 알고리즘을 사용해서 서로 밀어내도록 했습니다.

```cpp
  // Subsystem Tick 일부 Grid 공간 분할
	TMap<FIntVector, TArray<FEntityInstanceData*>> EntityGrid;
	EntityGrid.Reserve(256);

	for (auto& PoolPair : EntityPools)
	{
		FEntityPool& Pool = PoolPair.Value;
		for (FEntityInstanceData& Instance : Pool.Instances)
		{
			// Grid상의 같은 위치에 몇 개의 Entity 인스턴스가 있는지 계산하는 과정입니다.
			const FVector& Location = Instance.Transform.GetLocation();
			const FIntVector GridPos(
				FMath::FloorToInt(Location.X / GridCellSize),
				FMath::FloorToInt(Location.Y / GridCellSize),
				0
			);
			EntityGrid.FindOrAdd(GridPos).Add(&Instance);
		}
	}
```

## Object Pool

방금 위에서 설명한 Entity, ACharacter 기반의 Enemy, Ability가 스폰하는 Actor 등 다양하게 사용했습니다.

```cpp
AActor* UObjectPoolManagerSubsystem::GetFromPool(const TSubclassOf<AActor>& InClass, bool& bIsSpawning, AActor* OwnerActor, APawn* InstigatorPawn)
{
	TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InClass).InactiveActors;
	
	// Pool 안에 객체 수가 부족하면 만들어서 반환합니다.
	if (ActorPool.IsEmpty())
	{
		bIsSpawning = true;

		const FTransform SpawnTransform = FTransform(PoolLocation);
		AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(
			InClass,
			SpawnTransform,
			OwnerActor,
			InstigatorPawn,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		return SpawnedActor;
	}

	// Pool 안에 객체가 있다면 그대로 반환합니다.
	bIsSpawning = false;
	AActor* PopActor = ActorPool.Pop();

	// Tick은 필요하지 않을 수 있으므로, 관리 객체 혹은 객체 자체가 켜서 사용합니다.
	PopActor->SetActorHiddenInGame(false);
	PopActor->SetActorEnableCollision(true);
	return PopActor;
}

void UObjectPoolManagerSubsystem::ReturnToPool(AActor* InActor)
{
	if (InActor)
	{
		TArray<TObjectPtr<AActor>>& ActorPool = ActorPoolMap.FindOrAdd(InActor->GetClass()).InactiveActors;

		// Enemy 객체인 경우 Pool 안에 50개가 넘지 않도록 조정합니다.
		if (InActor->IsA(ASVEnemy::StaticClass()) && ActorPool.Num() > 50)
		{
			return;
		}

		// 게임 성능에 영향을 줄 수 있는 건 모두 끕니다.
		InActor->SetActorTickEnabled(false);
		InActor->SetActorHiddenInGame(true);
		InActor->SetActorEnableCollision(false);
		InActor->SetActorLocation(PoolLocation);
		
		ActorPool.Emplace(InActor);
	}
}
```
