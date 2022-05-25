#include "HealthAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "SaiyoraGAS/AbilitySystem/Components/SaiyoraAbilityComponent.h"

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
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Absorb, COND_None, REPNOTIFY_Always);
}

void UHealthAttributeSet::SetupDelegates()
{
	SETUP_NOTIFIER(UHealthAttributeSet, Health);
	SETUP_NOTIFIER(UHealthAttributeSet, MaxHealth);
	SETUP_NOTIFIER(UHealthAttributeSet, Absorb);
}

void UHealthAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
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
