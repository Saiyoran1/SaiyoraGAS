#include "SaiyoraCombatComponent.h"
#include "AbilitySystemInterface.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/SaiyoraGameplayAbility.h"
#include "SaiyoraGAS/AbilitySystem/Effects/SaiyoraGameplayEffect.h"

USaiyoraCombatComponent::USaiyoraCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaiyoraCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		check(GetOwner()->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
		const IAbilitySystemInterface* OwnerAbilitySystem = Cast<IAbilitySystemInterface>(GetOwner());
		AbilityComponent = Cast<USaiyoraAbilityComponent>(OwnerAbilitySystem->GetAbilitySystemComponent());
		check(AbilityComponent);
		AbilityComponent->InitAttributes(DefaultAttributes);
		for (const TTuple<TSubclassOf<USaiyoraGameplayAbility>, bool>& AbilityPair : DefaultAbilities)
		{
			if (AbilityPair.Key)
			{
				const FGameplayAbilitySpec Spec = FGameplayAbilitySpec(AbilityPair.Key, 1, INDEX_NONE, this);
				const FGameplayAbilitySpecHandle SpecHandle = AbilityComponent->GiveAbility(Spec);
				GrantedAbilities.Add(SpecHandle);
				if (AbilityPair.Value && SpecHandle.IsValid())
				{
					AbilityComponent->TryActivateAbility(SpecHandle);
				}
			}
		}
		for (const TSubclassOf<USaiyoraGameplayEffect> EffectClass : DefaultEffects)
		{
			if (EffectClass)
			{
				const USaiyoraGameplayEffect* DefaultEffect = EffectClass->GetDefaultObject<USaiyoraGameplayEffect>();
				AbilityComponent->ApplyGameplayEffectToSelf(DefaultEffect, 1.0f, AbilityComponent->MakeEffectContext());
			}
		}
	}
}




