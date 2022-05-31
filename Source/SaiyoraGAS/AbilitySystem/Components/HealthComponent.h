#pragma once
#include "CoreMinimal.h"
#include "SaiyoraCombatComponent.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/EventStructs/DamageStructs.h"
#include "HealthComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class SAIYORAGAS_API UHealthComponent : public USaiyoraCombatComponent
{
	GENERATED_BODY()

public:

	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return bIsAlive; }
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;
	UPROPERTY(BlueprintAssignable)
	FOnLifeStatusChanged OnLifeStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamageTaken;
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnHealingTaken;
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnAbsorbTaken;
	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamageDone;
	UPROPERTY(BlueprintAssignable)
	FOnKillingBlow OnKillingBlow;

	void AuthNotifyDamageTakenEvent(const FDamagingEvent& NewEvent);
	void ReplicatedNotifyDamageTakenEvent(const FDamagingEvent& NewEvent, const float EventTime);

	void AuthNotifyDamageDoneEvent(const FDamagingEvent& NewEvent);
	void ReplicatedNotifyDamageDoneEvent(const FDamagingEvent& NewEvent, const float EventTime);

	void AuthNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target);
	void ReplicatedNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target, const float EventTime);
	

private:

	static const float DAMAGEEVENTNOTIFYWINDOW;
	static const int32 MAXSAVEDDAMAGEEVENTS;
	static const float KILLINGBLOWNOTIFYWINDOW;
	static const int32 MAXSAVEDKILLINGBLOWS;

	virtual void PostInitialize() override;

	UPROPERTY(EditAnywhere, Category = "Health", meta = (AllowPrivateAccess))
	float BaseMaxHealth = 100.0f;
	void HealthChangedCallback(const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.OldValue, Data.NewValue); }
	void MaxHealthChangedCallback(const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.OldValue, Data.NewValue); }
	
	UPROPERTY(ReplicatedUsing=OnRep_IsAlive)
	bool bIsAlive = true;
	UFUNCTION()
	void OnRep_IsAlive();
	FGameplayAbilitySpecHandle DeathAbilityHandle;
	
	UPROPERTY(Replicated)
	FDamagingEventArray DamageTakenEvents;
	UPROPERTY(Replicated)
	FDamagingEventArray DamageDoneEvents;
	UPROPERTY(Replicated)
	FKillingBlowArray KillingBlows;
};
