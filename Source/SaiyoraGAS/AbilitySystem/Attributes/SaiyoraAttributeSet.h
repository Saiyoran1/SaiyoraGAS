#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
//This include is for the attribute accessor macros, so I don't have to include it in every attribute set.
#include "AbilitySystemComponent.h"
#include "SaiyoraAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeChange, const float, OldValue, const float, NewValue);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define ATTRIBUTE_NOTIFIER(ClassName, PropertyName) \
	void Fire##PropertyName##Delegate(const FOnAttributeChangeData& Data) \
	{ \
		On##PropertyName##Changed.Broadcast(Data.OldValue, Data.NewValue); \
	}

#define SETUP_NOTIFIER(ClassName, PropertyName) \
	GetOwningAbilityComponent()->GetGameplayAttributeValueChangeDelegate(Get##PropertyName##Attribute()).AddUObject(this, &##ClassName##::Fire##PropertyName##Delegate);

UCLASS(Abstract)
class SAIYORAGAS_API USaiyoraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	void SetOwningComponent(class USaiyoraAbilityComponent* AbilityComponent);
	UFUNCTION(BlueprintPure)
	class USaiyoraAbilityComponent* GetOwningAbilityComponent() const { return OwningAbilityComp; }
	
protected:

	
	void ScaleAttributeOnMaxChange(const FGameplayAttribute& Attribute, const float OldMax, const float NewMax);
	void ClampAttributeOnMaxChange(const FGameplayAttribute& Attribute, const float NewMax);

private:

	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const {}
	virtual void SetupDelegates() {}

	UPROPERTY(ReplicatedUsing=OnRep_OwningAbilityComp)
	class USaiyoraAbilityComponent* OwningAbilityComp;
	UFUNCTION()
	void OnRep_OwningAbilityComp();
};
