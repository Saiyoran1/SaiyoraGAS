#include "HealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/AbilityStructs.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"
#include "SaiyoraGAS/AbilitySystem/Components/SaiyoraAbilityComponent.h"

const float UHealthAttributeSet::MINMAXHEALTH = 1.0f;

UHealthAttributeSet::UHealthAttributeSet()
{
	MaxHealth = 100.0f;
	Health = 100.0f;
	Absorb = 0.0f;
	DamageTakenMultiplier = 1.0f;
	DamageTakenAddon = 0.0f;
	HealingTakenMultiplier = 1.0f;
	HealingTakenAddon = 0.0f;
	CrossPlaneDamageTakenMod = 1.0f;
	CrossPlaneHealingTakenMod = 1.0f;
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Absorb, COND_None, REPNOTIFY_Always);
}

void UHealthAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, MINMAXHEALTH);
	}
	else if (Attribute == GetAbsorbAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		ScaleAttributeOnMaxChange(GetHealthAttribute(), OldValue, NewValue);
		ClampAttributeOnMaxChange(GetAbsorbAttribute(), NewValue);
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if ((Data.EvaluatedData.Attribute == GetHealthAttribute() || Data.EvaluatedData.Attribute == GetAbsorbAttribute()) && Data.EvaluatedData.ModifierOp == EGameplayModOp::Additive && Data.EvaluatedData.Magnitude != 0.0f)
	{
		UHealthComponent* TargetHealthComp = Data.Target.GetOwner()->FindComponentByClass<UHealthComponent>();
		UHealthComponent* SourceHealthComp = Data.EffectSpec.GetEffectContext().GetInstigator()->GetOwner()->FindComponentByClass<UHealthComponent>();
		if (TargetHealthComp || SourceHealthComp)
		{
			FHealthEvent HealthEvent;
			HealthEvent.EventType = Data.EvaluatedData.Magnitude < 0.0f ? EHealthEventType::Damage : Data.EvaluatedData.Attribute == GetAbsorbAttribute() ? EHealthEventType::Absorb : EHealthEventType::Healing;
			HealthEvent.Attacker = Cast<USaiyoraAbilityComponent>(Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent());
			HealthEvent.Target = Cast<USaiyoraAbilityComponent>(&Data.Target);
			HealthEvent.HitStyle = FAbilityTags::DefaultHitStyle;
			for (const FGameplayTag Tag : Data.EffectSpec.CapturedSourceTags.GetSpecTags())
			{
				if (Tag.MatchesTag(FAbilityTags::HitStyle) && !Tag.MatchesTagExact(FAbilityTags::HitStyle))
				{
					HealthEvent.HitStyle = Tag;
					break;
				}
			}
			HealthEvent.School = FAbilityTags::DefaultSchool;
			for (const FGameplayTag Tag : Data.EffectSpec.CapturedSourceTags.GetSpecTags())
			{
				if (Tag.MatchesTag(FAbilityTags::School) && !Tag.MatchesTagExact(FAbilityTags::School))
				{
					HealthEvent.School = Tag;
					break;
				}
			}
			HealthEvent.Amount = HealthEvent.EventType == EHealthEventType::Damage ? -Data.EvaluatedData.Magnitude : Data.EvaluatedData.Magnitude;
			if (SourceHealthComp)
			{
				SourceHealthComp->AuthNotifyHealthEventDone(HealthEvent);
			}
			if (TargetHealthComp)
			{
				TargetHealthComp->AuthNotifyHealthEventTaken(HealthEvent);
			}
		}
	}
}

void UHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Health, Old);
}

void UHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxHealth, Old);
}

void UHealthAttributeSet::OnRep_Absorb(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Absorb, Old);
}
