#include "DamageExecution.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"
#include "SaiyoraGAS/AbilitySystem/Components/SaiyoraAbilityComponent.h"

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
	
	if (Spec.CapturedTargetTags.GetActorTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Status.DamageImmunity")))) &&
		!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Damage.Special.BypassImmunity")))))
	{
		return;
	}
	
	float Damage = FMath::Max(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.BaseDamage"), false), false, -1.0f), 0.0f);
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	if (!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Damage.Special.BypassModifiers")))))
	{
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
		Damage = FMath::Max(Damage * PlaneDamageDoneModifier * OutMultiplier + OutAddon, 0.0f);
	
		float InMultiplier = 1.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageTakenMultiplierDef, EvaluationParams, InMultiplier);
		float InAddon = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().DamageTakenAddonDef, EvaluationParams, InAddon);
		Damage = FMath::Max(Damage * PlaneDamageTakenModifier * InMultiplier + InAddon, 0.0f);
	}
	

	if (Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Damage.Special.BypassAbsorb")))))
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().HealthProperty, EGameplayModOp::Additive, -Damage));
	}
	else
	{
		float Absorb = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCapture().AbsorbDef, EvaluationParams, Absorb);
		if (Damage > Absorb)
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().AbsorbProperty, EGameplayModOp::Override, 0.0f));
			Damage -= Absorb;
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().HealthProperty, EGameplayModOp::Additive, -Damage));
		}
		else
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageCapture().AbsorbProperty, EGameplayModOp::Additive, -Damage));
		}
	}
	
	if (Damage > 0.0f)
	{
		UHealthComponent* TargetHealthComp = ExecutionParams.GetTargetAbilitySystemComponent()->GetOwner()->FindComponentByClass<UHealthComponent>();
		UHealthComponent* SourceHealthComp = ExecutionParams.GetSourceAbilitySystemComponent()->GetOwner()->FindComponentByClass<UHealthComponent>();
		if (TargetHealthComp || SourceHealthComp)
		{
			FHealthEvent DamageEvent;
			DamageEvent.EventType = EHealthEventType::Damage;
			DamageEvent.Attacker = Cast<USaiyoraAbilityComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
			DamageEvent.Target = Cast<USaiyoraAbilityComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
			const FGameplayTagContainer HitStyleFilter = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.HitStyle")), false));
			DamageEvent.HitStyle = Spec.CapturedSourceTags.GetSpecTags().Filter(HitStyleFilter).First();
			if (!DamageEvent.HitStyle.IsValid())
			{
				DamageEvent.HitStyle = FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.HitStyle.Default")), false);
			}
			const FGameplayTagContainer SpellTypeFilter = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.Type")), false));
			DamageEvent.SpellType = Spec.CapturedSourceTags.GetSpecTags().Filter(SpellTypeFilter).First();
			if (!DamageEvent.SpellType.IsValid())
			{
				DamageEvent.SpellType = FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.Type.Default")), false);
			}
			DamageEvent.Amount = Damage;
			if (TargetHealthComp)
			{
				TargetHealthComp->AuthNotifyHealthEventTaken(DamageEvent);
			}
			if (SourceHealthComp)
			{
				SourceHealthComp->AuthNotifyHealthEventDone(DamageEvent);
				if (TargetHealthComp && Damage > TargetHealthComp->GetHealth())
				{
					SourceHealthComp->AuthNotifyKillingBlowEvent(DamageEvent.Target);
				}
			}
		}
	}
	
}
