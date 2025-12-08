// KJY

#include "ExecCalc_Damage.h"

#include "Survivor/AbilitySystem/AttributeSet/SVAttributeSet.h"

// 해당 cpp 파일에서만 사용하는 원시 구조체로, 블루프린트는 물론 다른 클래스에서도 노출되지 않기 때문에 네이밍에 F를 붙이지 않습니다.
struct SVDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition SourceAttack;
	FGameplayEffectAttributeCaptureDefinition TargetArmor;

	SVDamageStatics() :
	SourceAttack(USVAttributeSet::GetAttackAttribute(), EGameplayEffectAttributeCaptureSource::Source, false),
	TargetArmor(USVAttributeSet::GetArmorAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
	{
	}
};

static const SVDamageStatics& DamageStatics()
{
	static SVDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().SourceAttack);
	RelevantAttributesToCapture.Add(DamageStatics().TargetArmor);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{	
	// 지금 적용 중인 GameplayEffectSpec을 가져옵니다.
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// 시전자와 피격자의 태그 정보를 가져와서 평가 파라미터에 세팅합니다.
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float SourceAttack = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().SourceAttack, EvaluationParameters, SourceAttack);

	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().TargetArmor, EvaluationParameters, TargetArmor);

	float Damage = SourceAttack - TargetArmor;
	Damage = FMath::Clamp(Damage, 0.f, Damage);

	// IncomingDamage Attribute에 Damage만큼 Additive 연산을 적용하라는 Modifier 데이터를 생성합니다.
	const FGameplayModifierEvaluatedData EvaluatedData(USVAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	// 이번 ExecCalc의 결과로 Modifier를 Output에 추가합니다.
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
