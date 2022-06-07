#include "SaiyoraMovementComponent.h"

#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"

void USaiyoraMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	HealthComponentRef = GetOwner()->FindComponentByClass<UHealthComponent>();
	HealthComponentRef->OnLifeStatusChanged.AddDynamic(this, &USaiyoraMovementComponent::OnLifeStatusChanged);
}

void USaiyoraMovementComponent::OnLifeStatusChanged(const bool bIsAlive)
{
	if (bIsAlive)
	{
		SetMovementMode(DefaultLandMovementMode);
	}
	else
	{
		StopMovementImmediately();
		DisableMovement();
	}
}
