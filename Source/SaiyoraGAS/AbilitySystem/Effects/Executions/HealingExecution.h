#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "HealingExecution.generated.h"

UCLASS()
class SAIYORAGAS_API UHealingExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UHealingExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
