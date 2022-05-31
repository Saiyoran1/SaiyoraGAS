#include "HealingExecution.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"

struct HealingCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingTakenAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealingDoneAddon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneHealingDoneMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CrossPlaneHealingTakenMod);

	HealingCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenMultiplier, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, HealingTakenAddon, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneMultiplier, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, HealingDoneAddon, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UDamageAttributeSet, CrossPlaneHealingDoneMod, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, CrossPlaneHealingTakenMod, Target, false);
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
	RelevantAttributesToCapture.Add(GetHealingCapture().CrossPlaneHealingTakenModDef);
	RelevantAttributesToCapture.Add(GetHealingCapture().CrossPlaneHealingDoneModDef);
}

void UHealingExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	if (Spec.CapturedTargetTags.GetActorTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Status.HealingImmunity")))) &&
			!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Healing.Special.BypassImmunity")))))
	{
		return;
	}
	
	float Healing = FMath::Max(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.BaseHealing"), false), false, -1.0f), 0.0f);
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	if (!Spec.CapturedSourceTags.GetSpecTags().HasTagExact(FGameplayTag::RequestGameplayTag(FName(TEXT("Healing.Special.BypassModifiers")))))
	{
		float PlaneHealingDoneModifier = 1.0f;
		float PlaneHealingTakenModifier = 1.0f;
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
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().CrossPlaneHealingDoneModDef, EvaluationParams, PlaneHealingDoneModifier);
				ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().CrossPlaneHealingTakenModDef, EvaluationParams, PlaneHealingTakenModifier);
			}
		}
		
		float OutMultiplier = 1.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingDoneMultiplierDef, EvaluationParams, OutMultiplier);
		float OutAddon = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingDoneAddonDef, EvaluationParams, OutAddon);
		Healing = FMath::Max(Healing * OutMultiplier * PlaneHealingDoneModifier + OutAddon, 0.0f);
	
		float InMultiplier = 1.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingTakenMultiplierDef, EvaluationParams, InMultiplier);
		float InAddon = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealingCapture().HealingTakenAddonDef, EvaluationParams, InAddon);
		Healing = FMath::Max(Healing * InMultiplier * PlaneHealingTakenModifier + InAddon, 0.0f);
	}
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetHealingCapture().HealthProperty, EGameplayModOp::Additive, Healing));

	if (Healing > 0.0f)
	{
		UHealthComponent* TargetHealthComp = ExecutionParams.GetTargetAbilitySystemComponent()->GetOwner()->FindComponentByClass<UHealthComponent>();
		UHealthComponent* SourceHealthComp = ExecutionParams.GetSourceAbilitySystemComponent()->GetOwner()->FindComponentByClass<UHealthComponent>();
		if (TargetHealthComp || SourceHealthComp)
		{
			FHealthEvent HealingEvent;
			HealingEvent.EventType = EHealthEventType::Healing;
			HealingEvent.Attacker = Cast<USaiyoraAbilityComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
			HealingEvent.Target = Cast<USaiyoraAbilityComponent>(ExecutionParams.GetSourceAbilitySystemComponent());
			const FGameplayTagContainer HitStyleFilter = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.HitStyle")), false));
			HealingEvent.HitStyle = Spec.CapturedSourceTags.GetSpecTags().Filter(HitStyleFilter).First();
			if (!HealingEvent.HitStyle.IsValid())
			{
				HealingEvent.HitStyle = FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.HitStyle.Default")), false);
			}
			const FGameplayTagContainer HealingTypeFilter = FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.Type")), false));
			HealingEvent.SpellType = Spec.CapturedSourceTags.GetSpecTags().Filter(HealingTypeFilter).First();
			if (!HealingEvent.SpellType.IsValid())
			{
				HealingEvent.SpellType = FGameplayTag::RequestGameplayTag(FName(TEXT("Spell.Type.Default")), false);
			}
			HealingEvent.Amount = Healing;
			if (TargetHealthComp)
			{
				TargetHealthComp->AuthNotifyHealthEventTaken(HealingEvent);
			}
			if (SourceHealthComp)
			{
				SourceHealthComp->AuthNotifyHealthEventDone(HealingEvent);
			}
		}
	}
}