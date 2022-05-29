#pragma once
#include "CoreMinimal.h"
#include "SaiyoraAttributeSet.h"
#include "EventStructs/DamageStructs.h"
#include "SaiyoraGAS/AbilitySystem/Components/SaiyoraAbilityComponent.h"
#include "DamageAttributeSet.generated.h"

UCLASS()
class SAIYORAGAS_API UDamageAttributeSet : public USaiyoraAttributeSet
{
	GENERATED_BODY()
	
public:

	UDamageAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageDoneMultiplier;
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, DamageDoneMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageDoneAddon;
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, DamageDoneAddon);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData CrossPlaneDamageDoneMod;
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, CrossPlaneDamageDoneMod);

	UPROPERTY(BlueprintReadOnly, Category = "Healing")
	FGameplayAttributeData HealingDoneMultiplier;
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, HealingDoneMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "Healing")
	FGameplayAttributeData HealingDoneAddon;
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, HealingDoneAddon);

	void AuthNotifyDamageDoneEvent(const FDamagingEvent& DamageEvent);
	void ReplicatedNotifyDamageDoneEvent(const FDamagingEvent& DamageEvent, const float EventTime);
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamageDone;

	void AuthNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target);
	void ReplicatedNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target, const float EventTime);
	UPROPERTY(BlueprintAssignable)
	FOnKillingBlow OnKillingBlow;

private:

	static const float DamageDoneNotifyWindow;
	static const int32 MaxSavedDamageDoneEvents;
	static const float KillingBlowNotifyWindow;
	static const int32 MaxSavedKillingBlows;

	virtual void SetupDelegates() override;

	UPROPERTY(Replicated)
	FDamagingEventArray DamageDoneEvents;
	UPROPERTY(Replicated)
	FKillingBlowArray KillingBlows;
};
