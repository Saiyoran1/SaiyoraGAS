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
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetAbsorb() const;
	UFUNCTION(BlueprintPure, Category = "Respawn")
	bool CanRespawn() const { return bRespawnEnabled; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Respawn")
	void RequestRespawn();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Respawn")
	void SetNewRespawnLocation(const FVector& NewLocation);
	UPROPERTY(BlueprintAssignable)
	FOnLifeStatusChanged OnLifeStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnRespawnStatusChanged OnRespawnStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable)
	FOnAbsorbChanged OnAbsorbChanged;
	UPROPERTY(BlueprintAssignable)
	FOnHealthEvent OnDamageTaken;
	UPROPERTY(BlueprintAssignable)
	FOnHealthEvent OnDamageDone;
	UPROPERTY(BlueprintAssignable)
	FOnKillingBlow OnKillingBlow;
	UPROPERTY(BlueprintAssignable)
	FOnHealthEvent OnHealingTaken;
	UPROPERTY(BlueprintAssignable)
	FOnHealthEvent OnHealingDone;
	UPROPERTY(BlueprintAssignable)
	FOnHealthEvent OnAbsorbTaken;
	UPROPERTY(BlueprintAssignable)
	FOnHealthEvent OnAbsorbDone;

	void AuthNotifyHealthEventTaken(const FHealthEvent& NewEvent);
	void ReplicatedNotifyHealthEventTaken(const FHealthEvent& NewEvent, const float EventTime);

	void AuthNotifyHealthEventDone(const FHealthEvent& NewEvent);
	void ReplicatedNotifyHealthEventDone(const FHealthEvent& NewEvent, const float EventTime);

	void AuthNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target);
	void ReplicatedNotifyKillingBlowEvent(USaiyoraAbilityComponent* Target, const float EventTime);
	

private:

	static const float HEALTHEVENTNOTIFYWINDOW;
	static const int32 MAXSAVEDHEALTHEVENTS;
	static const float KILLINGBLOWNOTIFYWINDOW;
	static const int32 MAXSAVEDKILLINGBLOWS;

	virtual void PostInitialize() override;

	UPROPERTY(EditAnywhere, Category = "Health")
	float BaseMaxHealth = 100.0f;
	void HealthChangedCallback(const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.OldValue, Data.NewValue); }
	void MaxHealthChangedCallback(const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.OldValue, Data.NewValue); }
	void AbsorbChangedCallback(const FOnAttributeChangeData& Data) { OnAbsorbChanged.Broadcast(Data.OldValue, Data.NewValue); }
	
	UPROPERTY(ReplicatedUsing=OnRep_IsAlive)
	bool bIsAlive = true;
	UFUNCTION()
	void OnRep_IsAlive();
	FGameplayAbilitySpecHandle DeathAbilityHandle;
	void OnDeathTagChanged(const FGameplayTag CallbackTag, const int32 NewCount);

	UPROPERTY(EditAnywhere, Category = "Respawn")
	bool bCanRespawn = false;
	UPROPERTY(EditAnywhere, Category = "Respawn")
	float RespawnDelay = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Respawn")
	bool bRespawnInPlace = true;
	UPROPERTY(ReplicatedUsing=OnRep_RespawnEnabled)
	bool bRespawnEnabled = false;
	UFUNCTION()
	void OnRep_RespawnEnabled();
	void EnableRespawn();
	FTimerHandle RespawnHandle;
	FVector RespawnLocation;
	
	UPROPERTY(Replicated)
	FHealthEventArray HealthEventsTaken;
	UPROPERTY(Replicated)
	FHealthEventArray HealthEventsDone;
	UPROPERTY(Replicated)
	FKillingBlowArray KillingBlows;

	UPROPERTY(EditAnywhere, Category = "Kill Count", meta = (AllowPrivateAccess, Categories = "KillCount"))
	FGameplayTag KillCountTag;
};
