#include "SaiyoraPlayerController.h"
#include "SaiyoraGameState.h"
#include "SaiyoraPlayerCharacter.h"
#include "SaiyoraPlayerState.h"
#include "GameFramework/GameStateBase.h"

void ASaiyoraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
	GameStateRef = GetWorld()->GetGameState() ? Cast<ASaiyoraGameState>(GetWorld()->GetGameState()) : nullptr;
	if (IsLocalController() && GetLocalRole() != ROLE_Authority)
	{
		SendPing();
		PingDelegate.BindUObject(this, &ASaiyoraPlayerController::SendPing);
		GetWorld()->GetTimerManager().SetTimer(PingHandle, PingDelegate, 5.0f, true);
	}
}

void ASaiyoraPlayerController::AcknowledgePossession(APawn* P)
{
	//This should fire on both client and server.
	Super::AcknowledgePossession(P);
	if (ASaiyoraPlayerCharacter* PC = Cast<ASaiyoraPlayerCharacter>(P))
	{
		PC->GetAbilityComponent()->InitAbilityActorInfo(PC, PC);
	}
}

void ASaiyoraPlayerController::SendPing()
{
	if (GameStateRef)
	{
		ServerSendClientTime(GameStateRef->GetServerWorldTimeSeconds());
	}
}

void ASaiyoraPlayerController::ServerSendClientTime_Implementation(const float ClientTime)
{
	if (GameStateRef)
	{
		ClientSendServerTime(GameStateRef->GetServerWorldTimeSeconds(), ClientTime);
	}
}

void ASaiyoraPlayerController::ClientSendServerTime_Implementation(const float ServerTime,
	const float ClientRequestTime)
{
	if (GameStateRef)
	{
		const float Adjustment = (GameStateRef->GetServerWorldTimeSeconds() - ClientRequestTime) / 2;
		GameStateRef->ReportAdjustedServerTime(ServerTime + Adjustment);
	}
}
