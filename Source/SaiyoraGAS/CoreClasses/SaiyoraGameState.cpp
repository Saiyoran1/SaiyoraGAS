#include "SaiyoraGameState.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

const float ASaiyoraGameState::CountdownLength = 10.0f;

#pragma region Boilerplate

ASaiyoraGameState::ASaiyoraGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void ASaiyoraGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASaiyoraGameState, ReadyPlayers);
	DOREPLIFETIME(ASaiyoraGameState, DungeonPhase);
}

#pragma endregion
#pragma region Time Sync

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

#pragma endregion
#pragma region Player Handling

void ASaiyoraGameState::InitPlayer(ASaiyoraPlayerCharacter* Player)
{
	if (!Player || GroupPlayers.Contains(Player))
	{
		return;
	}
	GroupPlayers.Add(Player);
	OnPlayerAdded.Broadcast(Player);
}

void ASaiyoraGameState::MarkPlayerReady(ASaiyoraPlayerCharacter* Player)
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::WaitingToStart || !Player || ReadyPlayers.Contains(Player))
	{
		return;
	}
	const TArray<ASaiyoraPlayerCharacter*> PreviousPlayers = ReadyPlayers;
	ReadyPlayers.Add(Player);
	OnRep_ReadyPlayers(PreviousPlayers);
	if (ReadyPlayers.Num() == GroupPlayers.Num())
	{
		if (const UMultiplayerSessionsSubsystem* SessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>())
		{
			FOnlineSessionSettings SessionSettings = SessionsSubsystem->GetSessionSettings(NAME_GameSession);
			SessionSettings.bAllowJoinInProgress = false;
			SessionsSubsystem->UpdateSession(NAME_GameSession, SessionSettings);
		}
		StartCountdown();
	}
}

void ASaiyoraGameState::MarkPlayerNotReady(ASaiyoraPlayerCharacter* Player)
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::WaitingToStart || !Player || !ReadyPlayers.Contains(Player))
	{
		return;
	}
	const TArray<ASaiyoraPlayerCharacter*> PreviousPlayers = ReadyPlayers;
	ReadyPlayers.Remove(Player);
	OnRep_ReadyPlayers(PreviousPlayers);
}

void ASaiyoraGameState::OnRep_ReadyPlayers(const TArray<ASaiyoraPlayerCharacter*>& PreviousPlayers)
{
	TArray<ASaiyoraPlayerCharacter*> UncheckedPlayers = PreviousPlayers;
	for (ASaiyoraPlayerCharacter* Player : ReadyPlayers)
	{
		if (PreviousPlayers.Contains(Player))
		{
			UncheckedPlayers.Remove(Player);
		}
		else
		{
			OnPlayerReadyChanged.Broadcast(Player, true);
		}
	}
	for (const ASaiyoraPlayerCharacter* Player : UncheckedPlayers)
	{
		OnPlayerReadyChanged.Broadcast(Player, false);
	}
}

#pragma endregion
#pragma region Match Flow

void ASaiyoraGameState::InitDungeonState()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::None)
	{
		return;
	}
	const FDungeonPhase OldPhase = DungeonPhase;
	DungeonPhase.DungeonState = EDungeonState::WaitingToStart;
	OnRep_DungeonPhase(OldPhase);
}

void ASaiyoraGameState::StartCountdown()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::WaitingToStart)
	{
		return;
	}
	const FDungeonPhase OldPhase = DungeonPhase;
	DungeonPhase.DungeonState = EDungeonState::Countdown;
	DungeonPhase.PhaseStartTime = WorldTime;
	DungeonPhase.PhaseEndTime = DungeonPhase.PhaseStartTime + CountdownLength;
	FTimerDelegate CountdownDel;
	CountdownDel.BindUObject(this, &ASaiyoraGameState::EndCountdown);
	GetWorld()->GetTimerManager().SetTimer(CountdownHandle, CountdownDel, CountdownLength, false);
	OnRep_DungeonPhase(OldPhase);
}

void ASaiyoraGameState::EndCountdown()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::Countdown)
	{
		return;
	}
	const FDungeonPhase OldPhase = DungeonPhase;
	DungeonPhase.DungeonState = EDungeonState::InProgress;
	DungeonPhase.PhaseStartTime = WorldTime;
	DungeonPhase.PhaseEndTime = DungeonPhase.PhaseStartTime + DungeonPhase.DungeonLength;
	OnRep_DungeonPhase(OldPhase);
}

void ASaiyoraGameState::OnRep_DungeonPhase(const FDungeonPhase& OldPhase)
{
	if (!bInitialized)
	{
		if (const FDungeonInfo* InfoPtr = DungeonInfoTable.Find(UGameplayStatics::GetCurrentLevelName(this)))
		{
			DungeonInfo = *InfoPtr;
			DungeonPhase.DungeonLength = DungeonInfo.TimeLimit;
			bInitialized = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No dungeon info found in GameState init!"));
		}
	}
	OnDungeonStateChanged.Broadcast(OldPhase, DungeonPhase);
}

#pragma endregion 