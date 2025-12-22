// KJY

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"

class UGameplayEffectApplier;
class USphereComponent;
class AFloatingActor;

DECLARE_DELEGATE_OneParam(FOnFloatingActorActivateSignature, const TArray<AActor*>& InActors);
DECLARE_DELEGATE_OneParam(FOnLifeEndSignature, AActor* InActor)

UCLASS()
class SURVIVOR_API AFloatingActor : public AActor
{
	GENERATED_BODY()

public:
	AFloatingActor(const FObjectInitializer& ObjectInitializer);

	void SetLifeTime(const float InLifeTime);

	void Activate(const FVector& InActivateLocation, const FRotator& InActivateRotation);

	UFUNCTION(BlueprintImplementableEvent)
	void OnActivate();

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End of AActor Interface

private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

public:
	FOnFloatingActorActivateSignature OnFloatingActorActivateDelegate;
	FOnLifeEndSignature OnLifeEndDelegate;

protected:
	// Ability가 Activate 시점에 넘겨주는 위치 정보로, 블루프린트의 Tick에서 활용해 해당 Actor의 움직임을 구현하는 데에 사용합니다.
	UPROPERTY(BlueprintReadOnly, Category = "FloatingActor")
	FVector TargetLocation;
	
	UPROPERTY(BlueprintReadOnly, Category = "FloatingActor")
	FRotator TargetRotation;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UShapeComponent> CollisionComponent;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> OverlappedActors;
	
	FTimerHandle LifeTimerHandle;
	FTimerHandle DamageTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "FloatingActor")
	float DamageFrequency = 0.5f;
};
