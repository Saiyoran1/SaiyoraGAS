#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SaiyoraAbilityComponent.generated.h"

USTRUCT(BlueprintType)
struct FAttributeInitialValues
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Attributes")
	TMap<FGameplayAttribute, float> Values;
};

UCLASS()
class SAIYORAGAS_API USaiyoraAbilityComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	void InitAttributes(const TMap<TSubclassOf<class USaiyoraAttributeSet>, FAttributeInitialValues>& Attributes);
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TMap<TSubclassOf<class USaiyoraAttributeSet>, FAttributeInitialValues> DefaultAttributes;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TMap<TSubclassOf<class USaiyoraGameplayAbility>, bool> DefaultAbilities;
	UPROPERTY()
	TArray<class USaiyoraCombatComponent*> CombatComponents;
};