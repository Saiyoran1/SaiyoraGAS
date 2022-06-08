#include "DeathAbility.h"
#include "AbilitySystemComponent.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Effects/DeathEffect.h"

const FGameplayTag UDeathAbility::DeathImmunity = FGameplayTag::RequestGameplayTag(FName(TEXT("Status.DeathImmunity")), false);

UDeathAbility::UDeathAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UDeathAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                       const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const FGameplayEffectContextHandle EffectContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpec DeathEffectSpec = FGameplayEffectSpec(UDeathEffect::StaticClass()->GetDefaultObject<UGameplayEffect>(), EffectContextHandle, 1.0f);
	ActorInfo->AbilitySystemComponent.Get()->ApplyGameplayEffectSpecToSelf(DeathEffectSpec);
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
