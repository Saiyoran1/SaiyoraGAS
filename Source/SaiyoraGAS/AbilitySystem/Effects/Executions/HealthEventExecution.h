#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "HealthEventExecution.generated.h"

UCLASS()
class SAIYORAGAS_API UHealthEventExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UHealthEventExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};


