#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SaiyoraCombatTags.generated.h"

USTRUCT()
struct FSaiyoraCombatTags
{
	GENERATED_BODY()

	static const FGameplayTag HealthEventType;
	static const FGameplayTag DamageEvent;
	static const FGameplayTag HealingEvent;
	static const FGameplayTag AbsorbEvent;
	static const FGameplayTag DamageImmunity;
	static const FGameplayTag HealingImmunity;
	static const FGameplayTag BypassImmunities;
	static const FGameplayTag BypassModifiers;
	static const FGameplayTag BypassCrossPlane;
	static const FGameplayTag BypassAbsorbs;
	static const FGameplayTag HealthEventBaseValue;

	static const FGameplayTag Plane;
	static const FGameplayTag AncientPlane;
	static const FGameplayTag ModernPlane;

	static const FGameplayTag Dead;
	static const FGameplayTag DeathImmunity;

	static const FGameplayTag AbilityIgnoreDeath;
};
