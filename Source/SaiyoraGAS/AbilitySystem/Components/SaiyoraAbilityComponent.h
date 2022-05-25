#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SaiyoraAbilityComponent.generated.h"

UCLASS()
class SAIYORAGAS_API USaiyoraAbilityComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	void InitAttributes(const TMap<TSubclassOf<class USaiyoraAttributeSet>, TSubclassOf<class USaiyoraGameplayEffect>>& Attributes);
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TMap<TSubclassOf<class USaiyoraAttributeSet>, TSubclassOf<class USaiyoraGameplayEffect>> DefaultAttributes;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TMap<TSubclassOf<class USaiyoraGameplayAbility>, bool> DefaultAbilities;
};