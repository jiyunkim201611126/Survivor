// KJY

#pragma once

#include "GameplayEffectTypes.h"
#include "SVAbilityTypes.Generated.h"

USTRUCT(BlueprintType)
struct FDamageDataContext
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize KnockbackForce = FVector_NetQuantize::ZeroVector;

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
};

/**
 * GameplayEffect부여나 GameplayCue 재생 등 다양한 로직에서 서버와 클라이언트간 데이터 공유가 필요할 때 사용되는 구조체입니다. 
 * 예를 들어 GE가 부여될 때 Ability로 적용한 건지, 그렇다면 어떤 Ability인지, GE를 적용시킨 건 누구인지 등등을 확인할 수 있습니다.
 * 이외에도 커스텀해 멤버 변수를 추가해 사용할 수 있습니다.
 */

USTRUCT(BlueprintType)
struct FSVGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	const FDamageDataContext& GetDamageData() const { return DamageData; }

	void SetKnockbackForce(const FVector& Force);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FSVGameplayEffectContext* NewContext = new FSVGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	// Context를 서버와 클라이언트 간 공유하기 위해 데이터를 직렬화 및 역직렬화하는 함수입니다.
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	FDamageDataContext DamageData;
};

/**
 * USTRUCT 자체만으로는 Replicate 시스템 같은 걸 이용할 수 없습니다.
 * 따라서 그것과 비슷하게 직렬화, 역직렬화 등의 기능을 활용해 서버와 클라이언트간 struct로 이루어진 데이터를 공유해야 합니다.
 * 아래 구문이 그걸 가능하게 해줍니다.
 * 작성된 2개의 열거자 외에도 다양한 열거자가 있으며, 서버와 클라이언트간 데이터 공유 시 유용하게 사용할 수 있는 기능들을 on/off합니다.
 */

template<>
struct TStructOpsTypeTraits<FSVGameplayEffectContext> : TStructOpsTypeTraitsBase2<FSVGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,	// 네트워크 직렬화 지원
		WithCopy = true				// 복사 연산 지원
	};
};
