#include "DeathEffect.h"
#include "SaiyoraGAS/AbilitySystem/Tags/SaiyoraCombatTags.h"

UDeathEffect::UDeathEffect()
{
	InheritableOwnedTagsContainer.Added.AddTag(FSaiyoraCombatTags::Dead);
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	UpdateInheritedTagProperties();
}
