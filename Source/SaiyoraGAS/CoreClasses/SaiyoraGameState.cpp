#include "SaiyoraGameState.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

const float ASaiyoraGameState::CountdownLength = 10.0f;
const FGameplayTag ASaiyoraGameState::GenericBossTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Boss")), false);

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
	DOREPLIFETIME(ASaiyoraGameState, DungeonProgress);
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
	const FTimerDelegate DepletionDelegate = FTimerDelegate::CreateUObject(this, &ASaiyoraGameState::DepleteDungeon);
	GetWorld()->GetTimerManager().SetTimer(DepletionHandle, DepletionDelegate, DungeonRequirements.TimeLimit, false);
}

void ASaiyoraGameState::MoveToCompletedState()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::InProgress)
	{
		return;
	}
	const FDungeonPhase OldPhase = DungeonPhase;
	GetWorld()->GetTimerManager().ClearTimer(DepletionHandle);
	DungeonPhase.DungeonState = EDungeonState::Completed;
	DungeonPhase.PhaseStartTime = WorldTime;
	DungeonPhase.PhaseEndTime = 0.0f;
	OnRep_DungeonPhase(OldPhase);
}

void ASaiyoraGameState::OnRep_DungeonPhase(const FDungeonPhase& OldPhase)
{
	if (!bInitialized)
	{
		if (const FDungeonRequirements* InfoPtr = DungeonInfoTable.Find(UGameplayStatics::GetCurrentLevelName(this)))
		{
			DungeonRequirements = *InfoPtr;
			DungeonPhase.DungeonLength = DungeonRequirements.TimeLimit;
			if (HasAuthority())
			{
				for (const FGameplayTag BossTag : DungeonRequirements.BossKillTags)
				{
					DungeonProgress.BossKills.Add(FBossProgress(BossTag, false));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No dungeon info found in GameState init!"));
		}
		bInitialized = true;
	}
	OnDungeonPhaseChanged.Broadcast(OldPhase, DungeonPhase);
}

#pragma endregion
#pragma region Progress

void ASaiyoraGameState::ReportTrashDeath(const int32 CountToAdd)
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::InProgress || CountToAdd <= 0)
	{
		return;
	}
	const FDungeonProgress PreviousProgress = DungeonProgress;
	DungeonProgress.KillCount += CountToAdd;
	OnUpdatedProgress(PreviousProgress);
}

void ASaiyoraGameState::ReportBossDeath(const FGameplayTag BossTag)
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::InProgress || !BossTag.IsValid() || !BossTag.MatchesTag(GenericBossTag) || BossTag.MatchesTagExact(GenericBossTag))
	{
		return;
	}
	for (int i = 0; i < DungeonProgress.BossKills.Num(); i++)
	{
		if (DungeonProgress.BossKills[i].BossTag.MatchesTagExact(BossTag) && !DungeonProgress.BossKills[i].bKilled)
		{
			const FDungeonProgress PreviousProgress = DungeonProgress;
			DungeonProgress.BossKills[i].bKilled = true;
			OnUpdatedProgress(PreviousProgress);
			break;
		}
	}
}

void ASaiyoraGameState::ReportPlayerDeath()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::InProgress)
	{
		return;
	}
	const FDungeonProgress PreviousProgress = DungeonProgress;
	DungeonProgress.DeathCount++;
	if (GetWorld()->GetTimerManager().IsTimerActive(DepletionHandle))
	{
		const float TimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(DepletionHandle);
		GetWorld()->GetTimerManager().ClearTimer(DepletionHandle);
		const float NewTimeRemaining = TimeRemaining - DeathPenaltySeconds;
		if (NewTimeRemaining <= 0.0f)
		{
			DepleteDungeon();
		}
		else
		{
			const FTimerDelegate DepletionDelegate = FTimerDelegate::CreateUObject(this, &ASaiyoraGameState::DepleteDungeon);
			GetWorld()->GetTimerManager().SetTimer(DepletionHandle, DepletionDelegate, NewTimeRemaining, false);
		}
	}
	OnUpdatedProgress(PreviousProgress);
}

void ASaiyoraGameState::DepleteDungeon()
{
	if (!HasAuthority() || DungeonPhase.DungeonState != EDungeonState::InProgress)
	{
		return;
	}
	const FDungeonProgress PreviousProgress = DungeonProgress;
	DungeonProgress.bDepleted = true;
	OnUpdatedProgress(PreviousProgress);
}

void ASaiyoraGameState::OnUpdatedProgress(const FDungeonProgress& PreviousProgress)
{
	if (PreviousProgress.CompletionTime == 0.0f && DungeonProgress.KillCount >= DungeonRequirements.KillCountRequirement)
	{
		TArray<FGameplayTag> BossTags;
		DungeonRequirements.BossKillTags.GetGameplayTagArray(BossTags);
		for (const FBossProgress& BossProgress : DungeonProgress.BossKills)
		{
			if (BossProgress.bKilled)
			{
				BossTags.Remove(BossProgress.BossTag);
			}
		}
		if (BossTags.Num() <= 0)
		{
			DungeonProgress.CompletionTime = WorldTime - DungeonPhase.PhaseStartTime + (DeathPenaltySeconds * DungeonProgress.DeathCount);
			MoveToCompletedState();
		}
	}
	OnRep_DungeonProgress(PreviousProgress);
}

void ASaiyoraGameState::OnRep_DungeonProgress(const FDungeonProgress& PreviousProgress)
{
	if (PreviousProgress.KillCount != DungeonProgress.KillCount)
	{
		OnKillCountChanged.Broadcast(DungeonProgress.KillCount);
	}
	if (PreviousProgress.DeathCount != DungeonProgress.DeathCount)
	{
		OnDeathCountChanged.Broadcast(DungeonProgress.DeathCount);
	}
	TArray<FBossProgress> PreviousBosses = PreviousProgress.BossKills;
	for (const FBossProgress& BossProgress : DungeonProgress.BossKills)
	{
		for (int i = 0; i < PreviousBosses.Num(); i++)
		{
			if (PreviousBosses[i].BossTag == BossProgress.BossTag)
			{
				if (PreviousBosses[i].bKilled != BossProgress.bKilled)
				{
					OnBossKilled.Broadcast(BossProgress.BossTag);
				}
				PreviousBosses.RemoveAt(i);
				break;
			}
		}
	}
	if (PreviousProgress.bDepleted != DungeonProgress.bDepleted && DungeonProgress.bDepleted)
	{
		OnDungeonDepleted.Broadcast();
	}
	if (PreviousProgress.CompletionTime != DungeonProgress.CompletionTime && DungeonProgress.CompletionTime != 0.0f)
	{
		OnDungeonCompleted.Broadcast(DungeonProgress.CompletionTime);
	}
}

#pragma endregion
