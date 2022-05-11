#pragma once
#include "CoreMinimal.h"
#include "SaiyoraAbilityComponent.h"
#include "Components/ActorComponent.h"
#include "SaiyoraCombatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract)
class SAIYORAGAS_API USaiyoraCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USaiyoraCombatComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	USaiyoraAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }

private:

	UPROPERTY()
	USaiyoraAbilityComponent* AbilityComponent;
	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<USaiyoraAttributeSet>, TSubclassOf<USaiyoraGameplayEffect>> DefaultAttributes;
	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<USaiyoraGameplayAbility>, bool> DefaultAbilities;
	TArray<FGameplayAbilitySpecHandle> GrantedAbilities;
	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TSet<TSubclassOf<USaiyoraGameplayEffect>> DefaultEffects;
};
