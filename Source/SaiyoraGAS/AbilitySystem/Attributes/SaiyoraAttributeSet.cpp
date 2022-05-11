#include "SaiyoraAttributeSet.h"

void USaiyoraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttributes(Attribute, NewValue);
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
