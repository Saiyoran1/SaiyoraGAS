#include "DamageAttributeSet.h"

UDamageAttributeSet::UDamageAttributeSet()
{
	DamageDoneMultiplier = 1.0f;
	DamageDoneAddon = 0.0f;
	HealingDoneMultiplier = 1.0f;
	HealingDoneAddon = 0.0f;
	CrossPlaneDamageDoneMod = 1.0f;
	CrossPlaneHealingDoneMod = 1.0f;
}