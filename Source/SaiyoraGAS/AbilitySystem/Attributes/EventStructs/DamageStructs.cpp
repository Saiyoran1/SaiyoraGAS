#include "DamageStructs.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"


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


