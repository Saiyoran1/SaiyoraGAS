#include "PyromancerAttributeSet.h"
#include "Net/UnrealNetwork.h"

UPyromancerAttributeSet::UPyromancerAttributeSet()
{
	MaxEmbers = 10;
	Embers = 5;
}

void UPyromancerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UPyromancerAttributeSet, Embers, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPyromancerAttributeSet, MaxEmbers, COND_None, REPNOTIFY_Always);
}

void UPyromancerAttributeSet::ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetMaxEmbersAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
	else if (Attribute == GetEmbersAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxEmbers());
	}
}

void UPyromancerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxEmbersAttribute())
	{
		ScaleAttributeOnMaxChange(GetEmbersAttribute(), OldValue, NewValue);
	}
}

void UPyromancerAttributeSet::OnRep_Embers(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPyromancerAttributeSet, Embers, Old);
}

void UPyromancerAttributeSet::OnRep_MaxEmbers(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPyromancerAttributeSet, MaxEmbers, Old);
}
