#include "DamageStructs.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"

const FGameplayTag FHealthEventTags::EventType = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type"), false);
const FGameplayTag FHealthEventTags::DamageEvent = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type.Damage"), false);
const FGameplayTag FHealthEventTags::DamageImmunity = FGameplayTag::RequestGameplayTag(TEXT("Status.DamageImmunity"), false);
const FGameplayTag FHealthEventTags::HealingEvent = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type.Healing"), false);
const FGameplayTag FHealthEventTags::HealingImmunity = FGameplayTag::RequestGameplayTag(TEXT("Status.HealingImmunity"), false);
const FGameplayTag FHealthEventTags::AbsorbEvent = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type.Absorb"), false);
const FGameplayTag FHealthEventTags::BypassImmunities = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassImmunities"), false);
const FGameplayTag FHealthEventTags::BypassModifiers = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassModifiers"), false);
const FGameplayTag FHealthEventTags::BypassCrossPlane = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassCrossPlane"), false);
const FGameplayTag FHealthEventTags::BypassAbsorbs = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassAbsorbs"), false);
const FGameplayTag FHealthEventTags::BaseValue = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.BaseValue"), false);
const FGameplayTag FHealthEventTags::Plane = FGameplayTag::RequestGameplayTag(TEXT("Plane"), false);

void FHealthEventItem::PostReplicatedAdd(const FHealthEventArray& InArraySerializer)
{
	InArraySerializer.OnAdded(DamageEvent, Time);
}

void FHealthEventArray::OnAdded(const FHealthEvent& NewEvent, const float EventTime) const
{
	if (OwningHealthComp)
	{
		OwningHealthComp->ReplicatedNotifyHealthEventTaken(NewEvent, EventTime);
	}
}

void FKillingBlowItem::PostReplicatedAdd(const FKillingBlowArray& InArraySerializer)
{
	InArraySerializer.OnAdded(Target, Time);
}

void FKillingBlowArray::OnAdded(USaiyoraAbilityComponent* Target, const float EventTime) const
{
	if (OwningHealthComp)
	{
		OwningHealthComp->ReplicatedNotifyKillingBlowEvent(Target, EventTime);
	}
}


