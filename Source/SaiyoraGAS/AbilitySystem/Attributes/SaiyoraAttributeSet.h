#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
//This include is for the attribute accessor macros, so I don't have to include it in every attribute set.
#include "AbilitySystemComponent.h"
#include "SaiyoraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS(Abstract)
class SAIYORAGAS_API USaiyoraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
protected:

	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const {}
	void ScaleAttributeOnMaxChange(const FGameplayAttribute& Attribute, const float OldMax, const float NewMax);
	void ClampAttributeOnMaxChange(const FGameplayAttribute& Attribute, const float NewMax);
};
