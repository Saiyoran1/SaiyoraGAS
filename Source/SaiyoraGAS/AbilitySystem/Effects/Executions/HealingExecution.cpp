#include "HealingExecution.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"

struct HealingCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneAddon);

	HealingCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenAddon, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneAddon, Source, true);
	}
};

static HealingCapture& GetHealingCapture()
{
	static HealingCapture HealingCapture;
	return HealingCapture;
}

UHealingExecution::UHealingExecution()
{
	RelevantAttributesToCapture.Add(GetHealingCapture().HealthDef);
	RelevantAttributesToCapture.Add(GetHealingCapture().HealingTakenMultiplierDef);
	RelevantAttributesToCapture.Add(GetHealingCapture().HealingTakenAddonDef);
	RelevantAttributesToCapture.Add(GetHealingCapture().HealingDoneMultiplierDef);
	RelevantAttributesToCapture.Add(GetHealingCapture().HealingDoneAddonDef);
}

void UHealingExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const float BaseHealing = FMath::Max(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.BaseHealing")), false, -1.0f), 0.0f);
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float OutMultiplier = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingDoneMultiplierDef, EvaluationParams, OutMultiplier);
	float OutAddon = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingDoneAddonDef, EvaluationParams, OutAddon);
	const float OutgoingHealing = FMath::Max(BaseHealing * OutMultiplier + OutAddon, 0.0f);
	
	float InMultiplier = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingTakenMultiplierDef, EvaluationParams, InMultiplier);
	float InAddon = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingTakenAddonDef, EvaluationParams, InAddon);
	const float IncomingHealing = FMath::Max(OutgoingHealing * InMultiplier + InAddon, 0.0f);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealingCapture().HealthProperty, EGameplayModOp::Additive, IncomingHealing));
}