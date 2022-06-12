#include "SaiyoraCombatTags.h"

const FGameplayTag FSaiyoraCombatTags::HealthEventType = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type"), false);
const FGameplayTag FSaiyoraCombatTags::DamageEvent = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type.Damage"), false);
const FGameplayTag FSaiyoraCombatTags::DamageImmunity = FGameplayTag::RequestGameplayTag(TEXT("Status.DamageImmunity"), false);
const FGameplayTag FSaiyoraCombatTags::HealingEvent = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type.Healing"), false);
const FGameplayTag FSaiyoraCombatTags::HealingImmunity = FGameplayTag::RequestGameplayTag(TEXT("Status.HealingImmunity"), false);
const FGameplayTag FSaiyoraCombatTags::AbsorbEvent = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Type.Absorb"), false);
const FGameplayTag FSaiyoraCombatTags::BypassImmunities = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassImmunities"), false);
const FGameplayTag FSaiyoraCombatTags::BypassModifiers = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassModifiers"), false);
const FGameplayTag FSaiyoraCombatTags::BypassCrossPlane = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassCrossPlane"), false);
const FGameplayTag FSaiyoraCombatTags::BypassAbsorbs = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.Special.BypassAbsorbs"), false);
const FGameplayTag FSaiyoraCombatTags::HealthEventBaseValue = FGameplayTag::RequestGameplayTag(TEXT("HealthEvent.BaseValue"), false);
const FGameplayTag FSaiyoraCombatTags::Plane = FGameplayTag::RequestGameplayTag(TEXT("Plane"), false);
const FGameplayTag FSaiyoraCombatTags::AncientPlane = FGameplayTag::RequestGameplayTag(TEXT("Plane.Ancient"), false);
const FGameplayTag FSaiyoraCombatTags::ModernPlane = FGameplayTag::RequestGameplayTag(TEXT("Plane.Modern"), false);
const FGameplayTag FSaiyoraCombatTags::Dead = FGameplayTag::RequestGameplayTag(TEXT("Status.Dead"), false);
const FGameplayTag FSaiyoraCombatTags::DeathImmunity = FGameplayTag::RequestGameplayTag(TEXT("Status.DeathImmunity"), false);
const FGameplayTag FSaiyoraCombatTags::AbilityIgnoreDeath = FGameplayTag::RequestGameplayTag(TEXT("Ability.Special.IgnoreDeath"), false);
const FGameplayTag FSaiyoraCombatTags::KillCountType = FGameplayTag::RequestGameplayTag(TEXT("KillCount"), false);
const FGameplayTag FSaiyoraCombatTags::PlayerKillCount = FGameplayTag::RequestGameplayTag(TEXT("KillCount.Player"), false);
const FGameplayTag FSaiyoraCombatTags::TrashKillCount = FGameplayTag::RequestGameplayTag(TEXT("KillCount.Trash"), false);
const FGameplayTag FSaiyoraCombatTags::BossKillCount = FGameplayTag::RequestGameplayTag(TEXT("KillCount.Boss"), false);