#include "SaiyoraPlayerController.h"

#include "SaiyoraPlayerCharacter.h"

void ASaiyoraPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	if (ASaiyoraPlayerCharacter* PC = Cast<ASaiyoraPlayerCharacter>(P))
	{
		PC->GetAbilityComponent()->InitAbilityActorInfo(PC, PC);
	}
}

void ASaiyoraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}
