#include "SelfStackingEffectExecution.h"
#include "AbilitySystemComponent.h"

void USelfStackingEffectExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectSpec SingleStackSpec = Spec;
	SingleStackSpec.StackCount = 1;
	ExecutionParams.GetTargetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(SingleStackSpec);
}
