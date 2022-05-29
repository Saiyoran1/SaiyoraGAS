#include "DamageAttributeSet.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

const float UDamageAttributeSet::DamageDoneNotifyWindow = 1.0f;
const int32 UDamageAttributeSet::MaxSavedDamageDoneEvents = 100;
const float UDamageAttributeSet::KillingBlowNotifyWindow = 5.0f;
const int32 UDamageAttributeSet::MaxSavedKillingBlows = 20;

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
	DOREPLIFETIME(UDamageAttributeSet, KillingBlows);
}

void UDamageAttributeSet::SetupDelegates()
{
	DamageDoneEvents.OwningDamageSet = this;
	KillingBlows.OwningDamageSet = this;
}

void UDamageAttributeSet::AuthNotifyDamageDoneEvent(const FDamagingEvent& DamageEvent)
{
	if (DamageDoneEvents.Items.Num() > MaxSavedDamageDoneEvents)
	{
		for (int i = DamageDoneEvents.Items.Num() - 1; i >= 0; i--)
		{
			if (DamageDoneEvents.Items[i].Time + DamageDoneNotifyWindow < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
			{
				DamageDoneEvents.Items.RemoveAt(i);
				DamageDoneEvents.MarkArrayDirty();
			}
		}
	}
	FDamagingEventItem TimestampedEvent;
	TimestampedEvent.DamageEvent = DamageEvent;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	DamageDoneEvents.MarkItemDirty(DamageDoneEvents.Items.Add_GetRef(TimestampedEvent));
	OnDamageDone.Broadcast(DamageEvent);
}

void UDamageAttributeSet::ReplicatedNotifyDamageDoneEvent(const FDamagingEvent& DamageEvent, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + DamageDoneNotifyWindow)
	{
		return;
	}
	OnDamageDone.Broadcast(DamageEvent);
}

void UDamageAttributeSet::AuthNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target)
{
	if (KillingBlows.Items.Num() > MaxSavedKillingBlows)
	{
		for (int i = KillingBlows.Items.Num() - 1; i >= 0; i--)
		{
			if (KillingBlows.Items[i].Time + KillingBlowNotifyWindow < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
			{
				KillingBlows.Items.RemoveAt(i);
				KillingBlows.MarkArrayDirty();
			}
		}
	}
	FKillingBlowItem TimestampedEvent;
	TimestampedEvent.Target = Target;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	KillingBlows.MarkItemDirty(KillingBlows.Items.Add_GetRef(TimestampedEvent));
	OnKillingBlow.Broadcast(Target);
}

void UDamageAttributeSet::ReplicatedNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + KillingBlowNotifyWindow)
	{
		return;
	}
	OnKillingBlow.Broadcast(Target);
}
