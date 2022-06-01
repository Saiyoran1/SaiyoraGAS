#pragma once
#include "GameplayTagContainer.h"
#include "Engine/NetSerialization.h"
#include "DamageStructs.generated.h"

USTRUCT()
struct FHealthEventTags
{
	GENERATED_BODY()

	static const FGameplayTag EventType;
	
	static const FGameplayTag DamageEvent;
	static const FGameplayTag DamageImmunity;
	
	static const FGameplayTag HealingEvent;
	static const FGameplayTag HealingImmunity;
	
	static const FGameplayTag AbsorbEvent;
	
	static const FGameplayTag BypassImmunities;
	static const FGameplayTag BypassModifiers;
	static const FGameplayTag BypassCrossPlane;
	static const FGameplayTag BypassAbsorbs;
	
	static const FGameplayTag BaseValue;

	static const FGameplayTag Plane;
};

UENUM(BlueprintType)
enum class EHealthEventType : uint8
{
	None,
	Damage,
	Healing,
	Absorb
};

USTRUCT(BlueprintType)
struct FHealthEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EHealthEventType EventType = EHealthEventType::None;
	UPROPERTY(BlueprintReadOnly)
	class USaiyoraAbilityComponent* Attacker = nullptr;
	UPROPERTY(BlueprintReadOnly)
	class USaiyoraAbilityComponent* Target = nullptr;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag HitStyle = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag School = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly)
	float Amount = 0.0f;
};

USTRUCT()
struct FHealthEventItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FHealthEvent DamageEvent;
	UPROPERTY()
	float Time = 0.0f;

	void PostReplicatedAdd(const struct FHealthEventArray& InArraySerializer);
};

USTRUCT()
struct FHealthEventArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FHealthEventItem> Items;
	UPROPERTY(NotReplicated)
	class UHealthComponent* OwningHealthComp = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FHealthEventItem, FHealthEventArray>(Items, DeltaParms, *this);
	}

	void OnAdded(const FHealthEvent& NewEvent, const float EventTime) const;
};

template<>
struct TStructOpsTypeTraits<FHealthEventArray> : public TStructOpsTypeTraitsBase2<FHealthEventArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

USTRUCT()
struct FKillingBlowItem : public FFastArraySerializerItem
{
	GENERATED_BODY();

	UPROPERTY()
	USaiyoraAbilityComponent* Target = nullptr;
	UPROPERTY()
	float Time = 0.0f;

	void PostReplicatedAdd(const struct FKillingBlowArray& InArraySerializer);
};

USTRUCT()
struct FKillingBlowArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FKillingBlowItem> Items;
	UPROPERTY(NotReplicated)
	class UHealthComponent* OwningHealthComp = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FKillingBlowItem, FKillingBlowArray>(Items, DeltaParms, *this);
	}

	void OnAdded(USaiyoraAbilityComponent* Target, const float EventTime) const;
};

template<>
struct TStructOpsTypeTraits<FKillingBlowArray> : public TStructOpsTypeTraitsBase2<FKillingBlowArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthEvent, const FHealthEvent&, HealthEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillingBlow, const USaiyoraAbilityComponent*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLifeStatusChanged, const bool, bAlive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, const float, PreviousHealth, const float, NewHealth);