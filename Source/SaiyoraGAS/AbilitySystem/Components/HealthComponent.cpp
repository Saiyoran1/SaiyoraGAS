#include "HealthComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/CoreAbilities/DeathAbility.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/DamageAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"

const float UHealthComponent::HEALTHEVENTNOTIFYWINDOW = 1.0f;
const int32 UHealthComponent::MAXSAVEDHEALTHEVENTS = 100;
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
	DOREPLIFETIME(UHealthComponent, HealthEventsTaken);
	DOREPLIFETIME(UHealthComponent, HealthEventsDone);
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
	HealthEventsTaken.OwningHealthComp = this;
	HealthEventsDone.OwningHealthComp = this;
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

void UHealthComponent::AuthNotifyHealthEventTaken(const FHealthEvent& NewEvent)
{
	if (HealthEventsTaken.Items.Num() > MAXSAVEDHEALTHEVENTS)
	{
		for (int i = HealthEventsTaken.Items.Num() - 1; i >= 0; i--)
		{
			if (HealthEventsTaken.Items[i].Time + HEALTHEVENTNOTIFYWINDOW < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
			{
				HealthEventsTaken.Items.RemoveAt(i);
				HealthEventsTaken.MarkArrayDirty();
			}
		}
	}
	FHealthEventItem TimestampedEvent;
	TimestampedEvent.DamageEvent = NewEvent;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	HealthEventsTaken.MarkItemDirty(HealthEventsTaken.Items.Add_GetRef(TimestampedEvent));
	switch (NewEvent.EventType)
	{
		case EHealthEventType::None :
			break;
		case EHealthEventType::Damage :
			OnDamageTaken.Broadcast(NewEvent);
			break;
		case EHealthEventType::Healing :
			OnHealingTaken.Broadcast(NewEvent);
			break;
		case EHealthEventType::Absorb :
			OnAbsorbTaken.Broadcast(NewEvent);
			break;
		default :
			break;
	}
}

void UHealthComponent::ReplicatedNotifyHealthEventTaken(const FHealthEvent& NewEvent, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + HEALTHEVENTNOTIFYWINDOW)
	{
		return;
	}
	switch (NewEvent.EventType)
	{
		case EHealthEventType::None :
			break;
		case EHealthEventType::Damage :
			OnDamageTaken.Broadcast(NewEvent);
			break;
		case EHealthEventType::Healing :
			OnHealingTaken.Broadcast(NewEvent);
			break;
		case EHealthEventType::Absorb :
			OnAbsorbTaken.Broadcast(NewEvent);
			break;
		default :
			break;
	}
}

void UHealthComponent::AuthNotifyHealthEventDone(const FHealthEvent& NewEvent)
{
	if (HealthEventsDone.Items.Num() > MAXSAVEDHEALTHEVENTS)
	{
		for (int i = HealthEventsDone.Items.Num() - 1; i >= 0; i--)
		{
			if (HealthEventsDone.Items[i].Time + HEALTHEVENTNOTIFYWINDOW < GetWorld()->GetGameState()->GetServerWorldTimeSeconds())
			{
				HealthEventsDone.Items.RemoveAt(i);
				HealthEventsDone.MarkArrayDirty();
			}
		}
	}
	FHealthEventItem TimestampedEvent;
	TimestampedEvent.DamageEvent = NewEvent;
	TimestampedEvent.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	HealthEventsDone.MarkItemDirty(HealthEventsDone.Items.Add_GetRef(TimestampedEvent));
	switch (NewEvent.EventType)
	{
		case EHealthEventType::None :
			break;
		case EHealthEventType::Damage :
			OnDamageDone.Broadcast(NewEvent);
			break;
		case EHealthEventType::Healing :
			OnHealingDone.Broadcast(NewEvent);
			break;
		case EHealthEventType::Absorb :
			OnAbsorbDone.Broadcast(NewEvent);
			break;
		default :
			break;
	}
}

void UHealthComponent::ReplicatedNotifyHealthEventDone(const FHealthEvent& NewEvent, const float EventTime)
{
	if (GetWorld() && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > EventTime + HEALTHEVENTNOTIFYWINDOW)
	{
		return;
	}
	switch (NewEvent.EventType)
	{
		case EHealthEventType::None :
			break;
		case EHealthEventType::Damage :
			OnDamageDone.Broadcast(NewEvent);
			break;
		case EHealthEventType::Healing :
			OnHealingDone.Broadcast(NewEvent);
			break;
		case EHealthEventType::Absorb :
			OnAbsorbDone.Broadcast(NewEvent);
			break;
		default :
			break;
	}
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
