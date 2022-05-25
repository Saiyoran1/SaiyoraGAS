#include "DamageAttributeSet.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UDamageAttributeSet::UDamageAttributeSet()
{
	DamageDoneMultiplier = 1.0f;
	DamageDoneAddon = 0.0f;
	HealingDoneMultiplier = 1.0f;
	HealingDoneAddon = 0.0f;
	CrossPlaneDamageDoneMod = 1.0f;
}

void UDamageAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDamageAttributeSet, DamageDoneEvents);
}

void UDamageAttributeSet::SetupDelegates()
{
	DamageDoneEvents.OwningDamageSet = this;
}

void UDamageAttributeSet::AuthNotifyDamageDoneEvent(const FDamagingEvent& DamageEvent)
{
	FDamagingEventItem TimestampedEvent;
	TimestampedEvent.DamageEvent = DamageEvent;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	DamageDoneEvents.MarkItemDirty(DamageDoneEvents.Items.Add_GetRef(TimestampedEvent));
	OnDamageDone.Broadcast(DamageEvent);
}

void UDamageAttributeSet::ReplicatedNotifyDamageDoneEvent(const FDamagingEvent& DamageEvent, const float EventTime)
{
	//TODO: Time filtering.
	OnDamageDone.Broadcast(DamageEvent);
}
