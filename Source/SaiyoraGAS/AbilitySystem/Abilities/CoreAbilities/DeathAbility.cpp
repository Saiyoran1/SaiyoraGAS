#include "DeathAbility.h"
#include "AbilitySystemComponent.h"
#include "SaiyoraGAS/AbilitySystem/Effects/DeathEffect.h"
#include "SaiyoraGAS/AbilitySystem/Tags/SaiyoraCombatTags.h"
#include "SaiyoraGAS/CoreClasses/SaiyoraGameState.h"

UDeathAbility::UDeathAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	ActivationBlockedTags.AddTag(FSaiyoraCombatTags::DeathImmunity);
	ActivationBlockedTags.AddTag(FSaiyoraCombatTags::Dead);
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const FGameplayEffectContextHandle EffectContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpec DeathEffectSpec = FGameplayEffectSpec(UDeathEffect::StaticClass()->GetDefaultObject<UGameplayEffect>(), EffectContextHandle, 1.0f);
	ActorInfo->AbilitySystemComponent.Get()->ApplyGameplayEffectSpecToSelf(DeathEffectSpec);
	
	const FGameplayTagContainer IgnoreTagsContainer = FGameplayTagContainer(FSaiyoraCombatTags::AbilityIgnoreDeath);
	ActorInfo->AbilitySystemComponent.Get()->CancelAbilities(nullptr, &IgnoreTagsContainer, this);
	
	if (ASaiyoraGameState* GameStateRef = Cast<ASaiyoraGameState>(GetWorld()->GetGameState()))
	{
		//TODO: Check tags to find out if player, boss, or trash.
		GameStateRef->ReportPlayerDeath();
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
