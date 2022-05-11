#pragma once
#include "CoreMinimal.h"
#include "SaiyoraAttributeSet.h"
#include "PyromancerAttributeSet.generated.h"

UCLASS()
class SAIYORAGAS_API UPyromancerAttributeSet : public USaiyoraAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UPyromancerAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Embers)
	FGameplayAttributeData Embers;
	ATTRIBUTE_ACCESSORS(UPyromancerAttributeSet, Embers);
	UFUNCTION()
	void OnRep_Embers(const FGameplayAttributeData& Old);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxEmbers)
	FGameplayAttributeData MaxEmbers;
	ATTRIBUTE_ACCESSORS(UPyromancerAttributeSet, MaxEmbers);
	UFUNCTION()
	void OnRep_MaxEmbers(const FGameplayAttributeData& Old);
	
protected:

	virtual void ClampAttributes(const FGameplayAttribute& Attribute, float& NewValue) const override;
};
