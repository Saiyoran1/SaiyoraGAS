#include "SaiyoraCombatComponent.h"

#include "Net/UnrealNetwork.h"

USaiyoraCombatComponent::USaiyoraCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaiyoraCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USaiyoraCombatComponent, AbilityComponent);
}

void USaiyoraCombatComponent::InitCombatComponent(USaiyoraAbilityComponent* OwnerAbilityComponent)
{
	if (bInitialized || !GetOwner()->HasAuthority())
	{
		return;
	}
	checkf(OwnerAbilityComponent, TEXT("Invalid ability component in %s."), *GetName());
	AbilityComponent = OwnerAbilityComponent;
	bInitialized = true;
	PostInitialize();
}

void USaiyoraCombatComponent::OnRep_AbilityComponent()
{
	if (!bInitialized && AbilityComponent)
	{
		bInitialized = true;
		PostInitialize();
	}
}




