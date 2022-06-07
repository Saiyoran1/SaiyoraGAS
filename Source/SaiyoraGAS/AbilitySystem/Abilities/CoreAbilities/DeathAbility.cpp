#include "DeathAbility.h"
#include "AbilitySystemComponent.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Effects/DeathEffect.h"

UDeathAbility::UDeathAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

bool UDeathAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                       const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	return ActorInfo->AbilitySystemComponent.Get()->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute()) <= 0.0f;
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	const FGameplayEffectContextHandle EffectContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpec DeathEffectSpec = FGameplayEffectSpec(UDeathEffect::StaticClass()->GetDefaultObject<UGameplayEffect>(), EffectContextHandle, 1.0f);
	ActorInfo->AbilitySystemComponent.Get()->ApplyGameplayEffectSpecToSelf(DeathEffectSpec);
}
