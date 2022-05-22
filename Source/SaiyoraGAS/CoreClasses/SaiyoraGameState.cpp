#include "SaiyoraGameState.h"
#include "GameModes/SaiyoraGameMode.h"
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

void ASaiyoraGameState::InitDungeonState()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::None)
	{
		return;
	}
	const FDungeonInfo* InfoPtr = DungeonInfoTable.Find(UGameplayStatics::GetCurrentLevelName(this));
	if (!InfoPtr)
	{
		return;
	}
	DungeonInfo = *InfoPtr;
	const FDungeonPhase OldPhase = DungeonPhase;
	DungeonPhase.DungeonState = EDungeonState::WaitingToStart;
	DungeonPhase.DungeonLength = DungeonInfo.TimeLimit;
	OnRep_DungeonPhase(OldPhase);
}

void ASaiyoraGameState::MarkPlayerReady(ASaiyoraPlayerCharacter* Player)
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::WaitingToStart)
	{
		return;
	}
	ReadyPlayers.AddUnique(Player);
	OnRep_ReadyPlayers();
	if (ReadyPlayers.Num() == PlayerArray.Num())
	{
		//GameModeRef->PreventFurtherJoining();
		StartCountdown();
	}
}

void ASaiyoraGameState::OnRep_ReadyPlayers()
{
	OnReadyPlayersChanged.Broadcast(ReadyPlayers);	
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
	OnDungeonStateChanged.Broadcast(OldPhase, DungeonPhase);
}