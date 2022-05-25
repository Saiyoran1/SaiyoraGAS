#include "DamageStructs.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"


void FDamagingEventItem::PostReplicatedAdd(const FDamagingEventArray& InArraySerializer)
{
	InArraySerializer.OnAdded(DamageEvent, Time);
}

void FDamagingEventArray::OnAdded(const FDamagingEvent& NewEvent, const float EventTime) const
{
	if (OwningHealthSet)
	{
		OwningHealthSet->ReplicatedNotifyDamageTakenEvent(NewEvent, EventTime);
	}
	else if (OwningDamageSet)
	{
		OwningDamageSet->ReplicatedNotifyDamageDoneEvent(NewEvent, EventTime);
	}
}
