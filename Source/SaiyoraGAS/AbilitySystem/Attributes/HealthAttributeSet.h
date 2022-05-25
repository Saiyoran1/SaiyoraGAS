#pragma once
#include "CoreMinimal.h"
#include "SaiyoraAttributeSet.h"
#include "HealthAttributeSet.generated.h"

UCLASS()
class SAIYORAGAS_API UHealthAttributeSet : public USaiyoraAttributeSet
{
	GENERATED_BODY()

public:

	UHealthAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health);
	UPROPERTY(BlueprintAssignable, meta = (HideInDetailsView))
	FAttributeChange OnHealthChanged;
	ATTRIBUTE_NOTIFIER(UHealthAttributeSet, Health);
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& Old);

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth);
	UPROPERTY(BlueprintAssignable, meta = (HideInDetailsView))
	FAttributeChange OnMaxHealthChanged;
	ATTRIBUTE_NOTIFIER(UHealthAttributeSet, MaxHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& Old);

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Absorb)
	FGameplayAttributeData Absorb;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Absorb);
	UPROPERTY(BlueprintAssignable, meta = (HideInDetailsView))
	FAttributeChange OnAbsorbChanged;
	ATTRIBUTE_NOTIFIER(UHealthAttributeSet, Absorb);
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

private:

	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void SetupDelegates() override;
	
};
