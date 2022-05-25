#pragma once
#include "GameplayTagContainer.h"
#include "DamageStructs.generated.h"

USTRUCT(BlueprintType)
struct FDamagingEvent
{
	GENERATED_BODY()

	UPROPERTY()
	class USaiyoraAbilityComponent* Attacker;
	UPROPERTY()
	class USaiyoraAbilityComponent* Target;
	UPROPERTY()
	FGameplayTag HitStyle;
	UPROPERTY()
	FGameplayTag DamageType;
	UPROPERTY()
	float Damage;
};

USTRUCT()
struct FDamagingEventItem : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FDamagingEvent DamageEvent;
	UPROPERTY()
	float Time;

	void PostReplicatedAdd(const struct FDamagingEventArray& InArraySerializer);
};

USTRUCT()
struct FDamagingEventArray : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FDamagingEventItem> Items;
	UPROPERTY(NotReplicated)
	class UHealthAttributeSet* OwningHealthSet;
	UPROPERTY(NotReplicated)
	class UDamageAttributeSet* OwningDamageSet;

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, const FDamagingEvent&, DamageEvent);
