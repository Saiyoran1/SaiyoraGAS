#include "HealthEventExecution.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/AbilityStructs.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"
#include "SaiyoraGAS/AbilitySystem/Components/SaiyoraAbilityComponent.h"
#include "SaiyoraGAS/AbilitySystem/Tags/SaiyoraCombatTags.h"

struct HealthEventCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Absorb);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTakenMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTakenAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageDoneMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageDoneAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneDamageDoneMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneDamageTakenMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneHealingDoneMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneHealingTakenMod);

	HealthEventCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Absorb, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, DamageTakenMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, DamageTakenAddon, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, DamageDoneMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, DamageDoneAddon, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, CrossPlaneDamageDoneMod, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CrossPlaneDamageTakenMod, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenAddon, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneAddon, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, CrossPlaneHealingDoneMod, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CrossPlaneHealingTakenMod, Target, false);
	}
};

static HealthEventCapture& GetHealthEventCapture()
{
	static HealthEventCapture HealthEventCapture;
	return HealthEventCapture;
}

UHealthEventExecution::UHealthEventExecution()
{
	RelevantAttributesToCapture.Add(GetHealthEventCapture().HealthDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().AbsorbDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().DamageTakenMultiplierDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().DamageTakenAddonDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().DamageDoneMultiplierDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().DamageDoneAddonDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().CrossPlaneDamageDoneModDef);
	RelevantAttributesToCapture.Add(GetHealthEventCapture().CrossPlaneDamageTakenModDef);
}

void UHealthEventExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	FGameplayTag HealthEventTypeTag = FSaiyoraCombatTags::DamageEvent;
	TArray<FGameplayTag> SourceSpecTags;
	Spec.CapturedSourceTags.GetSpecTags().GetGameplayTagArray(SourceSpecTags);
	for (const FGameplayTag Tag : SourceSpecTags)
	{
		if (Tag.MatchesTag(FSaiyoraCombatTags::HealthEventType) && !Tag.MatchesTagExact(FSaiyoraCombatTags::HealthEventType))
		{
			HealthEventTypeTag = Tag;
			break;
		}
	}
	EHealthEventType EventType = EHealthEventType::None;
	if (HealthEventTypeTag == FSaiyoraCombatTags::DamageEvent)
	{
		EventType = EHealthEventType::Damage;
	}
	else if (HealthEventTypeTag == FSaiyoraCombatTags::HealingEvent)
	{
		EventType = EHealthEventType::Healing;
	}
	else
	{
		EventType = EHealthEventType::Absorb;
	}
	
	if (Spec.CapturedTargetTags.GetActorTags().HasTagExact(EventType == EHealthEventType::Damage ? FSaiyoraCombatTags::DamageImmunity : FSaiyoraCombatTags::HealingImmunity) &&
		!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FSaiyoraCombatTags::BypassImmunities))
	{
		return;
	}

	float Value = FMath::Max(Spec.GetSetByCallerMagnitude(FSaiyoraCombatTags::HealthEventBaseValue, false, -1.0f), 0.0f);
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	if (!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FSaiyoraCombatTags::BypassModifiers))
	{
		float OutMultiplier = 1.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EventType == EHealthEventType::Damage ? GetHealthEventCapture().DamageDoneMultiplierDef : GetHealthEventCapture().HealingDoneMultiplierDef,
			EvaluationParams, OutMultiplier);
		float OutAdditive = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EventType == EHealthEventType::Damage ? GetHealthEventCapture().DamageDoneAddonDef : GetHealthEventCapture().HealingDoneAddonDef,
			EvaluationParams, OutAdditive);
	
		float InMultiplier = 1.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EventType == EHealthEventType::Damage ? GetHealthEventCapture().DamageTakenMultiplierDef : GetHealthEventCapture().HealingTakenMultiplierDef,
			EvaluationParams, InMultiplier);
		float InAdditive = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EventType == EHealthEventType::Damage ? GetHealthEventCapture().DamageTakenAddonDef : GetHealthEventCapture().HealingTakenAddonDef,
			EvaluationParams, InAdditive);

		if (!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FSaiyoraCombatTags::BypassCrossPlane))
		{
			float OutCrossPlaneMultiplier = 1.0f;
			float InCrossPlaneMultiplier = 1.0f;
			FGameplayTag SourcePlane = FGameplayTag::EmptyTag;
			TArray<FGameplayTag> SourceActorTags;
			Spec.CapturedSourceTags.GetActorTags().GetGameplayTagArray(SourceActorTags);
			for (const FGameplayTag& Tag : SourceActorTags)
			{
				if (Tag.MatchesTag(FSaiyoraCombatTags::Plane) && !Tag.MatchesTagExact(FSaiyoraCombatTags::Plane))
				{
					SourcePlane = Tag;
					break;
				}
			}
			if (SourcePlane.IsValid())
			{
				FGameplayTag TargetPlane = FGameplayTag::EmptyTag;
				TArray<FGameplayTag> TargetActorTags;
				Spec.CapturedTargetTags.GetActorTags().GetGameplayTagArray(TargetActorTags);
				for (const FGameplayTag& Tag : TargetActorTags)
				{
					if (Tag.MatchesTag(FSaiyoraCombatTags::Plane) && !Tag.MatchesTagExact(FSaiyoraCombatTags::Plane))
					{	
						TargetPlane = Tag;
						break;
					}
				}
				if (TargetPlane.IsValid() && TargetPlane != SourcePlane)
				{
					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EventType == EHealthEventType::Damage ? GetHealthEventCapture().CrossPlaneDamageDoneModDef : GetHealthEventCapture().CrossPlaneHealingDoneModDef,
						EvaluationParams, OutCrossPlaneMultiplier);
					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(EventType == EHealthEventType::Damage ? GetHealthEventCapture().CrossPlaneDamageTakenModDef : GetHealthEventCapture().CrossPlaneHealingTakenModDef,
						EvaluationParams, InCrossPlaneMultiplier);
				}
			}
			OutMultiplier *= OutCrossPlaneMultiplier;
			InMultiplier *= InCrossPlaneMultiplier;
		}

		Value = FMath::Max(0.0f, (Value * OutMultiplier + OutAdditive) * InMultiplier + InAdditive);
	}

	switch (EventType)
	{
		case EHealthEventType::Damage :
			{
				if (Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FSaiyoraCombatTags::BypassAbsorbs))
				{
					OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealthEventCapture().HealthProperty, EGameplayModOp::Additive, -Value));
				}
				else
				{
					float Absorb = 0.0f;
					ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealthEventCapture().AbsorbDef, EvaluationParams, Absorb);
					if (Value > Absorb)
					{
						OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealthEventCapture().AbsorbProperty, EGameplayModOp::Additive, -Absorb));
						Value -= Absorb;
						OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealthEventCapture().HealthProperty, EGameplayModOp::Additive, -Value));
					}
					else
					{
						OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealthEventCapture().AbsorbProperty, EGameplayModOp::Additive, -Value));
					}
				}
			}
			break;
		case EHealthEventType::Healing :
			{
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealthEventCapture().HealthProperty, EGameplayModOp::Additive, Value));
			}
			break;
		case EHealthEventType::Absorb :
			{
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealthEventCapture().AbsorbProperty, EGameplayModOp::Additive, Value));
			}
			break;
		default :
			break;
	}
}
