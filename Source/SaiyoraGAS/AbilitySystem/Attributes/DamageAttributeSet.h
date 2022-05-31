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

	UPROPERTY(BlueprintReadOnly, Category = "Healing")
	FGameplayAttributeData CrossPlaneHealingDoneMod;
	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, CrossPlaneHealingDoneMod);
};
