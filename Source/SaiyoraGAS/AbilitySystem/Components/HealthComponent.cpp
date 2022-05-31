#include "HealthComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/CoreAbilities/DeathAbility.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"

const float UHealthComponent::DAMAGEEVENTNOTIFYWINDOW = 1.0f;
const int32 UHealthComponent::MAXSAVEDDAMAGEEVENTS = 100;
const float UHealthComponent::KILLINGBLOWNOTIFYWINDOW = 5.0f;
const int32 UHealthComponent::MAXSAVEDKILLINGBLOWS = 100;

UHealthComponent::UHealthComponent()
{
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, bIsAlive);
	DOREPLIFETIME(UHealthComponent, DamageTakenEvents);
	DOREPLIFETIME(UHealthComponent, DamageDoneEvents);
	DOREPLIFETIME(UHealthComponent, KillingBlows);
}

float UHealthComponent::GetHealth() const
{
	return GetAbilityComponent() ? GetAbilityComponent()->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute()) : 0.0f;
}

float UHealthComponent::GetMaxHealth() const
{
	return GetAbilityComponent() ? GetAbilityComponent()->GetNumericAttribute(UHealthAttributeSet::GetMaxHealthAttribute()) : 0.0f;
}

void UHealthComponent::PostInitialize()
{
	if (GetOwner()->HasAuthority())
	{
		FAttributeInitialValues InitialValues;
		const float ActualMaxHealth = FMath::Max(UHealthAttributeSet::MINMAXHEALTH, BaseMaxHealth);
		InitialValues.Values.Add(UHealthAttributeSet::GetMaxHealthAttribute(), ActualMaxHealth);
		InitialValues.Values.Add(UHealthAttributeSet::GetHealthAttribute(), ActualMaxHealth);
		TMap<TSubclassOf<USaiyoraAttributeSet>, FAttributeInitialValues> InitialValuesMap;
		InitialValuesMap.Add(UHealthAttributeSet::StaticClass(), InitialValues);
		InitialValuesMap.Add(UDamageAttributeSet::StaticClass(), FAttributeInitialValues());
		GetAbilityComponent()->InitAttributes(InitialValuesMap);
	}
	DamageTakenEvents.OwningHealthComp = this;
	DamageDoneEvents.OwningHealthComp = this;
	KillingBlows.OwningHealthComp = this;
	GetAbilityComponent()->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UHealthComponent::MaxHealthChangedCallback);
	GetAbilityComponent()->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &UHealthComponent::HealthChangedCallback);
	OnMaxHealthChanged.Broadcast(0.0f, GetAbilityComponent()->GetNumericAttribute(UHealthAttributeSet::GetMaxHealthAttribute()));
	OnHealthChanged.Broadcast(0.0f, GetAbilityComponent()->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute()));
}

void UHealthComponent::OnRep_IsAlive()
{
	OnLifeStatusChanged.Broadcast(bIsAlive);
}

void UHealthComponent::AuthNotifyDamageTakenEvent(const FDamagingEvent& NewEvent)
{
	if (DamageTakenEvents.Items.Num() > MAXSAVEDDAMAGEEVENTS)
	{
		for (int i = DamageTakenEvents.Items.Num() - 1; i >= 0; i--)
		{
			if (DamageTakenEvents.Items[i].Time + DAMAGEEVENTNOTIFYWINDOW < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
			{
				DamageTakenEvents.Items.RemoveAt(i);
				DamageTakenEvents.MarkArrayDirty();
			}
		}
	}
	FDamagingEventItem TimestampedEvent;
	TimestampedEvent.DamageEvent = NewEvent;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	DamageTakenEvents.MarkItemDirty(DamageTakenEvents.Items.Add_GetRef(TimestampedEvent));
	OnDamageTaken.Broadcast(NewEvent);
}

void UHealthComponent::ReplicatedNotifyDamageTakenEvent(const FDamagingEvent& NewEvent, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + DAMAGEEVENTNOTIFYWINDOW)
	{
		return;
	}
	OnDamageTaken.Broadcast(NewEvent);
}

void UHealthComponent::AuthNotifyDamageDoneEvent(const FDamagingEvent& NewEvent)
{
	if (DamageDoneEvents.Items.Num() > MAXSAVEDDAMAGEEVENTS)
	{
		for (int i = DamageDoneEvents.Items.Num() - 1; i >= 0; i--)
		{
			if (DamageDoneEvents.Items[i].Time + DAMAGEEVENTNOTIFYWINDOW < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
			{
				DamageDoneEvents.Items.RemoveAt(i);
				DamageDoneEvents.MarkArrayDirty();
			}
		}
	}
	FDamagingEventItem TimestampedEvent;
	TimestampedEvent.DamageEvent = NewEvent;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	DamageDoneEvents.MarkItemDirty(DamageDoneEvents.Items.Add_GetRef(TimestampedEvent));
	OnDamageDone.Broadcast(NewEvent);
}

void UHealthComponent::ReplicatedNotifyDamageDoneEvent(const FDamagingEvent& NewEvent, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + DAMAGEEVENTNOTIFYWINDOW)
	{
		return;
	}
	OnDamageDone.Broadcast(NewEvent);
}

void UHealthComponent::AuthNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target)
{
	if (KillingBlows.Items.Num() > MAXSAVEDKILLINGBLOWS)
	{
		for (int i = KillingBlows.Items.Num() - 1; i >= 0; i--)
		{
			if (KillingBlows.Items[i].Time + KILLINGBLOWNOTIFYWINDOW < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
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

void UHealthComponent::ReplicatedNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + KILLINGBLOWNOTIFYWINDOW)
	{
		return;
	}
	OnKillingBlow.Broadcast(Target);
}
