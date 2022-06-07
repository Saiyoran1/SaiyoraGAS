#include "DeathEffect.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"

UDeathEffect::UDeathEffect()
{
	InheritableOwnedTagsContainer.Added.AddTag(UHealthComponent::DeathTag);
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	UpdateInheritedTagProperties();
}
