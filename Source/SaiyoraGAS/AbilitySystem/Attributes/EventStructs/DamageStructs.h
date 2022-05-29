#pragma once
#include "GameplayTagContainer.h"
#include "Engine/NetSerialization.h"
#include "DamageStructs.generated.h"

USTRUCT(BlueprintType)
struct FDamagingEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	class USaiyoraAbilityComponent* Attacker = nullptr;
	UPROPERTY(BlueprintReadOnly)
	class USaiyoraAbilityComponent* Target = nullptr;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag HitStyle = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DamageType = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.0f;
};

USTRUCT()
struct FDamagingEventItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FDamagingEvent DamageEvent;
	UPROPERTY()
	float Time;

	void PostReplicatedAdd(const struct FDamagingEventArray& InArraySerializer);
};

USTRUCT()
struct FDamagingEventArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FDamagingEventItem> Items;
	UPROPERTY(NotReplicated)
	class UHealthAttributeSet* OwningHealthSet = nullptr;
	UPROPERTY(NotReplicated)
	class UDamageAttributeSet* OwningDamageSet = nullptr;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FDamagingEventItem, FDamagingEventArray>(Items, DeltaParms, *this);
	}

	void OnAdded(const FDamagingEvent& NewEvent, const float EventTime) const;
};

template<>
struct TStructOpsTypeTraits<FDamagingEventArray> : public TStructOpsTypeTraitsBase2<FDamagingEventArray>
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
	USaiyoraAbilityComponent* Target;
	UPROPERTY()
	float Time;

	void PostReplicatedAdd(const struct FKillingBlowArray& InArraySerializer);
};

USTRUCT()
struct FKillingBlowArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FKillingBlowItem> Items;
	UPROPERTY(NotReplicated)
	class UDamageAttributeSet* OwningDamageSet = nullptr;

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, const FDamagingEvent&, DamageEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillingBlow, const USaiyoraAbilityComponent*, Target);