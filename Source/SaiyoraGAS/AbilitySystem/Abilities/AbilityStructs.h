#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilityStructs.generated.h"

USTRUCT()
struct FAbilityTags
{
	GENERATED_BODY()

	static const FGameplayTag HitStyle;
	static const FGameplayTag DefaultHitStyle;
	static const FGameplayTag DirectHitStyle;
	static const FGameplayTag AreaHitStyle;
	static const FGameplayTag ChronicHitStyle;
	
	static const FGameplayTag School;
	static const FGameplayTag DefaultSchool;
	static const FGameplayTag PhysicalSchool;
	static const FGameplayTag FireSchool;
	static const FGameplayTag SkySchool;
	static const FGameplayTag WaterSchool;
	static const FGameplayTag EarthSchool;
};