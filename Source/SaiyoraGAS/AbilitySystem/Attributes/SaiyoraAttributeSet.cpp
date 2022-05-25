#include "SaiyoraAttributeSet.h"

#include "Net/UnrealNetwork.h"

void USaiyoraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USaiyoraAttributeSet, OwningAbilityComp);
}

void USaiyoraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttributes(Attribute, NewValue);
}

void USaiyoraAttributeSet::SetOwningComponent(USaiyoraAbilityComponent* AbilityComponent)
{
	OwningAbilityComp = AbilityComponent;
	OnRep_OwningAbilityComp();
}

void USaiyoraAttributeSet::ScaleAttributeOnMaxChange(const FGameplayAttribute& Attribute, const float OldMax,
                                                     const float NewMax)
{
	if (!GetOwningAbilitySystemComponent())
	{
		return;
	}
	GetOwningAbilitySystemComponent()->ApplyModToAttribute(Attribute, EGameplayModOp::Override, Attribute.GetNumericValue(this) * NewMax / OldMax);
}

void USaiyoraAttributeSet::ClampAttributeOnMaxChange(const FGameplayAttribute& Attribute, const float NewMax)
{
	if (!GetOwningAbilitySystemComponent())
	{
		return;
	}
	if (Attribute.GetNumericValue(this) > NewMax)
	{
		GetOwningAbilitySystemComponent()->ApplyModToAttribute(Attribute, EGameplayModOp::Override, NewMax);
	}
}

void USaiyoraAttributeSet::OnRep_OwningAbilityComp()
{
	if (OwningAbilityComp)
	{
		SetupDelegates();
	}
}
