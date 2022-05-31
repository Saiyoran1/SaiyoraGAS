#include "SaiyoraAbilityComponent.h"

#include "SaiyoraCombatComponent.h"
#include "SaiyoraGAS/AbilitySystem/Abilities/SaiyoraGameplayAbility.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/HealthAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Attributes/SaiyoraAttributeSet.h"
#include "SaiyoraGAS/AbilitySystem/Effects/SaiyoraGameplayEffect.h"

void USaiyoraAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		InitAttributes(DefaultAttributes);
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
		GetOwner()->GetComponents<USaiyoraCombatComponent>(CombatComponents);
		for (USaiyoraCombatComponent* Component : CombatComponents)
		{
			Component->InitCombatComponent(this);
		}
	}
}

void USaiyoraAbilityComponent::InitAttributes(
	const TMap<TSubclassOf<USaiyoraAttributeSet>, FAttributeInitialValues>& Attributes)
{
	if (GetOwner()->HasAuthority())
	{
		for (const TTuple<TSubclassOf<USaiyoraAttributeSet>, FAttributeInitialValues>& AttributeSet : Attributes)
		{
			const UAttributeSet* NewSet = GetOrCreateAttributeSubobject(AttributeSet.Key);
			USaiyoraAttributeSet* SaiyoraSet = const_cast<USaiyoraAttributeSet*>(Cast<USaiyoraAttributeSet>(NewSet));
			if (SaiyoraSet)
			{
				SaiyoraSet->SetOwningComponent(this);
			}
			for (const TTuple<FGameplayAttribute, float>& InitialValue : AttributeSet.Value.Values)
			{
				if (InitialValue.Key.GetAttributeSetClass() == NewSet->GetClass())
				{
					SetNumericAttributeBase(InitialValue.Key, InitialValue.Value);
				}
			}
		}
	}
}
