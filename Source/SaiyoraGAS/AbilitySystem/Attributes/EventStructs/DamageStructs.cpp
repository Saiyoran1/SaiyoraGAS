#include "DamageStructs.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"


void FDamagingEventItem::PostReplicatedAdd(const FDamagingEventArray& InArraySerializer)
{
	InArraySerializer.OnAdded(DamageEvent, Time);
}

void FDamagingEventArray::OnAdded(const FDamagingEvent& NewEvent, const float EventTime) const
{
	if (OwningHealthComp)
	{
		OwningHealthComp->ReplicatedNotifyDamageTakenEvent(NewEvent, EventTime);
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


