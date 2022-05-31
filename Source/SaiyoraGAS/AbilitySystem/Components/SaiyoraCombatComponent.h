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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void InitCombatComponent(USaiyoraAbilityComponent* OwnerAbilityComponent);

	UFUNCTION(BlueprintPure, Category = "Abilities")
	USaiyoraAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }

private:

	UPROPERTY(ReplicatedUsing=OnRep_AbilityComponent)
	USaiyoraAbilityComponent* AbilityComponent;
	UFUNCTION()
	void OnRep_AbilityComponent();
	bool bInitialized = false;
	
	virtual void PostInitialize() {}
};
