#include "DamageExecution.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"

struct DamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Absorb);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTakenMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTakenAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageDoneMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageDoneAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneDamageDoneMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneDamageTakenMod);

	DamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Absorb, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, DamageTakenMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, DamageTakenAddon, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, DamageDoneMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, DamageDoneAddon, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, CrossPlaneDamageDoneMod, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CrossPlaneDamageTakenMod, Target, false);
	}
};

static DamageCapture& GetDamageCapture()
{
	static DamageCapture DamageCapture;
	return DamageCapture;
}

UDamageExecution::UDamageExecution()
{
	RelevantAttributesToCapture.Add(GetDamageCapture().HealthDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().AbsorbDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageTakenMultiplierDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageTakenAddonDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageDoneMultiplierDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().DamageDoneAddonDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().CrossPlaneDamageDoneModDef);
	RelevantAttributesToCapture.Add(GetDamageCapture().CrossPlaneDamageTakenModDef);
}

void UDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const float BaseDamage = FMath::Max(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.BaseDamage")), false, -1.0f), 0.0f);
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float PlaneDamageDoneModifier = 1.0f;
	float PlaneDamageTakenModifier = 1.0f;
	FGameplayTag SourcePlane = FGameplayTag::EmptyTag;
	for (const FGameplayTag& Tag : Spec.CapturedSourceTags.GetActorTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Plane"), false)) && !Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Plane"), false)))
		{
			SourcePlane = Tag;
			break;
		}
	}
	if (SourcePlane.IsValid())
	{
		FGameplayTag TargetPlane = FGameplayTag::EmptyTag;
		for (const FGameplayTag& Tag : Spec.CapturedTargetTags.GetActorTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Plane"), false)) && !Tag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Plane"), false)))
			{
				TargetPlane = Tag;
				break;
			}
		}
		if (TargetPlane.IsValid() && TargetPlane != SourcePlane)
		{
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().CrossPlaneDamageDoneModDef, EvaluationParams, PlaneDamageDoneModifier);
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().CrossPlaneDamageTakenModDef, EvaluationParams, PlaneDamageTakenModifier);
		}
	}

	//TODO: Hitbox multiplier?
	
	float OutMultiplier = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageDoneMultiplierDef, EvaluationParams, OutMultiplier);
	float OutAddon = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageDoneAddonDef, EvaluationParams, OutAddon);
	const float OutgoingDamage = FMath::Max(BaseDamage * PlaneDamageDoneModifier * OutMultiplier + OutAddon, 0.0f);
	
	float InMultiplier = 1.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageTakenMultiplierDef, EvaluationParams, InMultiplier);
	float InAddon = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageTakenAddonDef, EvaluationParams, InAddon);
	float IncomingDamage = FMath::Max(OutgoingDamage * PlaneDamageTakenModifier * InMultiplier + InAddon, 0.0f);
	
	float Absorb = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().AbsorbDef, EvaluationParams, Absorb);
	if (IncomingDamage > Absorb)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().AbsorbProperty, EGameplayModOp::Override, 0.0f));
		IncomingDamage -= Absorb;
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().HealthProperty, EGameplayModOp::Additive, -IncomingDamage));
	}
	else
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().AbsorbProperty, EGameplayModOp::Additive, -IncomingDamage));
	}
	if (IncomingDamage > 0.0f)
	{
		//TODO: Is this how this should be done?
		/*FGameplayEventData EventData;
		EventData.Instigator = ExecutionParams.GetSourceAbilitySystemComponent()->GetOwner();
		EventData.Target = ExecutionParams.GetTargetAbilitySystemComponent()->GetOwner();
		EventData.EventMagnitude = IncomingDamage;
		EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.DamageDone"), false);
		EventData.InstigatorTags = Spec.CapturedSourceTags.GetActorTags();
		EventData.TargetTags = Spec.CapturedTargetTags.GetActorTags();
		ExecutionParams.GetSourceAbilitySystemComponent()->HandleGameplayEvent(EventData.EventTag, &EventData);
		EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.DamageTaken"), false);
		ExecutionParams.GetTargetAbilitySystemComponent()->HandleGameplayEvent(EventData.EventTag, &EventData);*/
	}
}
