#pragma once
#include "CoreMinimal.h"
#include "SaiyoraAttributeSet.h"
#include "EventStructs/DamageStructs.h"
#include "HealthAttributeSet.generated.h"

UCLASS()
class SAIYORAGAS_API UHealthAttributeSet : public USaiyoraAttributeSet
{
	GENERATED_BODY()

public:

	static const float MINMAXHEALTH;

	UHealthAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& Old);

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& Old);

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Absorb)
	FGameplayAttributeData Absorb;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Absorb);
	UFUNCTION()
	void OnRep_Absorb(const FGameplayAttributeData& Old);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTakenMultiplier;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, DamageTakenMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DamageTakenAddon;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, DamageTakenAddon);

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData CrossPlaneDamageTakenMod;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, CrossPlaneDamageTakenMod);

	UPROPERTY(BlueprintReadOnly, Category = "Healing")
	FGameplayAttributeData HealingTakenMultiplier;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, HealingTakenMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "Healing")
	FGameplayAttributeData HealingTakenAddon;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, HealingTakenAddon);

	UPROPERTY(BlueprintReadOnly, Category = "Healing")
	FGameplayAttributeData CrossPlaneHealingTakenMod;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, CrossPlaneHealingTakenMod);

private:
	
	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const override;
};
