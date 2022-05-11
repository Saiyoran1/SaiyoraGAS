#include "SaiyoraAbilityComponent.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/SaiyoraGameplayAbility.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/SaiyoraAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Effects/SaiyoraGameplayEffect.h"

void USaiyoraAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	InitAttributes(DefaultAttributes);
	if (GetOwner()->HasAuthority())
	{
		for (const TTuple<TSubclassOf<USaiyoraGameplayAbility>, bool>& AbilityClass : DefaultAbilities)
		{
			if (AbilityClass.Key)
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass.Key, 1, INDEX_NONE, this);
				const FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);
				if (AbilityClass.Value && Handle.IsValid())
				{
					TryActivateAbility(Handle);
				}
			}
		}
	}
}

void USaiyoraAbilityComponent::InitAttributes(
	const TMap<TSubclassOf<USaiyoraAttributeSet>, TSubclassOf<USaiyoraGameplayEffect>>& Attributes)
{
	if (GetOwner()->HasAuthority())
	{
		for (const TTuple<TSubclassOf<USaiyoraAttributeSet>, TSubclassOf<USaiyoraGameplayEffect>>& AttributeSet : Attributes)
		{
			GetOrCreateAttributeSubobject(AttributeSet.Key);
			if (AttributeSet.Value)
			{
				FGameplayEffectContextHandle EffectContext = MakeEffectContext();
				EffectContext.AddSourceObject(this);
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(AttributeSet.Value, 1, EffectContext);
				if (EffectSpecHandle.IsValid())
				{
					ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
				}
			}
		}
	}
}
