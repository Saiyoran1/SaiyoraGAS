#include "AbsorbExecution.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"

struct AbsorbCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Absorb);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneAddon);

	AbsorbCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Absorb, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenAddon, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneAddon, Source, true);
	}
};

static AbsorbCapture& GetAbsorbCapture()
{
	static AbsorbCapture AbsorbCapture;
	return AbsorbCapture;
}

UAbsorbExecution::UAbsorbExecution()
{
	RelevantAttributesToCapture.Add(GetAbsorbCapture().AbsorbDef);
	RelevantAttributesToCapture.Add(GetAbsorbCapture().HealingTakenMultiplierDef);
	RelevantAttributesToCapture.Add(GetAbsorbCapture().HealingTakenAddonDef);
	RelevantAttributesToCapture.Add(GetAbsorbCapture().HealingDoneMultiplierDef);
	RelevantAttributesToCapture.Add(GetAbsorbCapture().HealingDoneAddonDef);
}

void UAbsorbExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const float BaseAbsorb = FMath::Max(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.BaseAbsorb"), false), false, -1.0f), 0.0f);
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float OutMultiplier = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAbsorbCapture().HealingDoneMultiplierDef, EvaluationParams, OutMultiplier);
	float OutAddon = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAbsorbCapture().HealingDoneAddonDef, EvaluationParams, OutAddon);
	const float OutgoingAbsorb = FMath::Max(BaseAbsorb * OutMultiplier + OutAddon, 0.0f);
	
	float InMultiplier = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAbsorbCapture().HealingTakenMultiplierDef, EvaluationParams, InMultiplier);
	float InAddon = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAbsorbCapture().HealingTakenAddonDef, EvaluationParams, InAddon);
	const float IncomingAbsorb = FMath::Max(OutgoingAbsorb * InMultiplier + InAddon, 0.0f);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAbsorbCapture().AbsorbProperty, EGameplayModOp::Additive, IncomingAbsorb));
}