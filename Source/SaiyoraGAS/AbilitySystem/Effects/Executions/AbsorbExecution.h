#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbsorbExecution.generated.h"

UCLASS()
class SAIYORAGAS_API UAbsorbExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UAbsorbExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
