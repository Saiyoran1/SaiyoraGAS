#pragma once
#include "CoreMinimal.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/SaiyoraGameplayAbility.h"
#include "DeathAbility.generated.h"

UCLASS()
class SAIYORAGAS_API UDeathAbility : public USaiyoraGameplayAbility
{
	GENERATED_BODY()

public:

	static const FGameplayTag DeathImmunity;
	
	UDeathAbility();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
