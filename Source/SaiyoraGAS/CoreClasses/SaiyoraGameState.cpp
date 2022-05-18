#include "SaiyoraGameState.h"
#include "GameModes/SaiyoraGameMode.h"
#include "Net/UnrealNetwork.h"

const float ASaiyoraGameState::CountdownLength = 10.0f;

ASaiyoraGameState::ASaiyoraGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void ASaiyoraGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaiyoraGameState, ReadyPlayers);
	DOREPLIFETIME(ASaiyoraGameState, StartTime);
	DOREPLIFETIME(ASaiyoraGameState, GoalTime);
	DOREPLIFETIME(ASaiyoraGameState, DungeonState);
}

void ASaiyoraGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	WorldTime += DeltaSeconds;
}

float ASaiyoraGameState::GetServerWorldTimeSeconds() const
{
	return WorldTime;
}

void ASaiyoraGameState::ReportAdjustedServerTime(const float AdjustedTime)
{
	WorldTime = AdjustedTime;
}

void ASaiyoraGameState::SetDungeonState(const EDungeonState NewState)
{
	if (DungeonState == NewState)
	{
		return;
	}
	DungeonState = NewState;
	switch (DungeonState)
	{
	case EDungeonState::None :
		return;
	case EDungeonState::WaitingToStart :
		return;
	case EDungeonState::Countdown :
		StartCountdown();
		return;
	case EDungeonState::InProgress :
		return;
	case EDungeonState::Completed :
		return;
	}
	OnDungeonStateChanged.Broadcast(NewState);
}

void ASaiyoraGameState::MarkPlayerReady(ASaiyoraPlayerCharacter* Player)
{
	if (!Player->HasAuthority() || DungeonState != EDungeonState::WaitingToStart)
	{
		return;
	}
	ReadyPlayers.AddUnique(Player);
	OnReadyPlayersChanged.Broadcast(ReadyPlayers);
	if (ReadyPlayers.Num() == PlayerArray.Num())
	{
		GameModeRef->PreventFurtherJoining();
		SetDungeonState(EDungeonState::Countdown);
	}
}

void ASaiyoraGameState::StartCountdown()
{
	StartTime = GetServerWorldTimeSeconds() + CountdownLength;
	FTimerDelegate CountdownDel;
	CountdownDel.BindUObject(this, &ASaiyoraGameState::EndCountdown);
	GetWorld()->GetTimerManager().SetTimer(CountdownHandle, CountdownDel, CountdownLength, false);
	OnStartTimeChanged.Broadcast(StartTime);
}

void ASaiyoraGameState::EndCountdown()
{
	SetDungeonState(EDungeonState::InProgress);
}

void ASaiyoraGameState::OnRep_DungeonState()
{
	OnDungeonStateChanged.Broadcast(DungeonState);
}

void ASaiyoraGameState::OnRep_ReadyPlayers()
{
	OnReadyPlayersChanged.Broadcast(ReadyPlayers);	
}

void ASaiyoraGameState::OnRep_StartTime()
{
	if (StartTime != 0.0f)
	{
		OnStartTimeChanged.Broadcast(StartTime);
	}
}

void ASaiyoraGameState::OnRep_GoalTime()
{
	if (GoalTime != 0.0f)
	{
		OnGoalTimeChanged.Broadcast(GoalTime);
	}
}