#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SaiyoraPlayerCharacter.h"
#include "SaiyoraGameState.generated.h"

USTRUCT(BlueprintType)
struct FDungeonRequirements
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 KillCountRequirement = 0;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (Categories = "Boss"))
	FGameplayTagContainer BossKillTags;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float TimeLimit = 0.0f;
};

UENUM(BlueprintType)
enum class EDungeonState : uint8
{
	None,
	WaitingToStart,
	Countdown,
	InProgress,
	Completed
};

USTRUCT(BlueprintType)
struct FDungeonPhase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EDungeonState DungeonState = EDungeonState::None;
	UPROPERTY(BlueprintReadOnly)
	float PhaseStartTime = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float PhaseEndTime = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	float DungeonLength = 0.0f;
};

USTRUCT(BlueprintType)
struct FBossProgress
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag BossTag;
	UPROPERTY(BlueprintReadOnly)
	bool bKilled = false;

	FORCEINLINE bool operator==(const FBossProgress& Other) const { return Other.BossTag == BossTag; }
	FBossProgress() { BossTag = FGameplayTag::EmptyTag; bKilled = false; }
	FBossProgress(const FGameplayTag InitBossTag, const bool bInitKilled) { BossTag = InitBossTag; bKilled = bInitKilled; }
};

USTRUCT(BlueprintType)
struct FDungeonProgress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 KillCount = 0;
	UPROPERTY(BlueprintReadOnly)
	TArray<FBossProgress> BossKills;
	UPROPERTY(BlueprintReadOnly)
	int32 DeathCount = 0;
	UPROPERTY(BlueprintReadOnly)
	bool bDepleted = false;
	UPROPERTY(BlueprintReadOnly)
	float CompletionTime = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerChanged, const ASaiyoraPlayerCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerReadyChanged, const ASaiyoraPlayerCharacter*, Player, const bool, bReadyStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, const float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDungeonPhaseChanged, const FDungeonPhase&, OldState, const FDungeonPhase&, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossKilled, const FGameplayTag, BossTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountChanged, const int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDungeonCompleted, const float, FinalTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDungeonDepleted);

UCLASS()
class SAIYORAGAS_API ASaiyoraGameState : public AGameState
{
	GENERATED_BODY()

public:

	static const FGameplayTag GenericBossTag;

	ASaiyoraGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float GetServerWorldTimeSeconds() const override;
	void ReportAdjustedServerTime(const float AdjustedTime);

	void InitPlayer(ASaiyoraPlayerCharacter* Player);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void MarkPlayerReady(ASaiyoraPlayerCharacter* Player);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void MarkPlayerNotReady(ASaiyoraPlayerCharacter* Player);
	UPROPERTY(BlueprintAssignable)
	FOnPlayerReadyChanged OnPlayerReadyChanged;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerChanged OnPlayerAdded;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerChanged OnPlayerRemoved;
	UFUNCTION(BlueprintPure)
	void GetSaiyoraPlayers(TArray<ASaiyoraPlayerCharacter*>& OutPlayers) const { OutPlayers = GroupPlayers; }
	UFUNCTION(BlueprintPure)
	bool IsPlayerReady(const ASaiyoraPlayerCharacter* Player) const { return ReadyPlayers.Contains(Player); }

	void InitDungeonState();
	UPROPERTY(BlueprintAssignable)
	FOnDungeonPhaseChanged OnDungeonPhaseChanged;
	UFUNCTION(BlueprintPure)
	EDungeonState GetDungeonState() const { return DungeonPhase.DungeonState; }
	UFUNCTION(BlueprintPure)
	float GetDungeonStartTime() const { return DungeonPhase.PhaseStartTime; }
	UFUNCTION(BlueprintPure)
	float GetDungeonEndTime() const { return DungeonPhase.PhaseEndTime; }
	UFUNCTION(BlueprintPure)
	FDungeonPhase GetDungeonPhaseInfo() const { return DungeonPhase; }

	UFUNCTION(BlueprintPure)
	float GetDungeonTimeLimit() const { return DungeonRequirements.TimeLimit; }
	UFUNCTION(BlueprintPure)
	int32 GetDungeonKillCount() const { return DungeonRequirements.KillCountRequirement; }
	UFUNCTION(BlueprintPure)
	void GetBossKillRequirements(FGameplayTagContainer& OutTags) const { OutTags = DungeonRequirements.BossKillTags; }
	UFUNCTION(BlueprintPure)
	FDungeonRequirements GetDungeonRequirements() const { return DungeonRequirements; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ReportTrashDeath(const int32 CountToAdd = 1);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ReportBossDeath(const FGameplayTag BossTag);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ReportPlayerDeath();
	UFUNCTION(BlueprintPure)
	int32 GetCurrentKillCount() const { return DungeonProgress.KillCount; }
	UFUNCTION(BlueprintPure)
	void GetBossKillProgress(TArray<FBossProgress>& OutProgress) const { OutProgress = DungeonProgress.BossKills; }
	UFUNCTION(BlueprintPure)
	int32 GetDeathCount() const { return DungeonProgress.DeathCount; }
	UFUNCTION(BlueprintPure)
	bool IsDungeonDepleted() const { return DungeonProgress.bDepleted; }
	UFUNCTION(BlueprintPure)
	float GetDungeonCompletionTime() const { return DungeonProgress.CompletionTime; }
	UFUNCTION(BlueprintPure)
	FDungeonProgress GetDungeonProgress() const { return DungeonProgress; }
	UPROPERTY(BlueprintAssignable)
	FOnBossKilled OnBossKilled;
	UPROPERTY(BlueprintAssignable)
	FOnCountChanged OnKillCountChanged;
	UPROPERTY(BlueprintAssignable)
	FOnCountChanged OnDeathCountChanged;
	UPROPERTY(BlueprintAssignable)
	FOnDungeonCompleted OnDungeonCompleted;
	UPROPERTY(BlueprintAssignable)
	FOnDungeonDepleted OnDungeonDepleted;

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FDungeonRequirements> DungeonInfoTable;
	UPROPERTY(EditDefaultsOnly)
	float DeathPenaltySeconds = 5.0f;

private:

	float WorldTime = 0.0f;

	UPROPERTY(ReplicatedUsing=OnRep_DungeonPhase)
	FDungeonPhase DungeonPhase;
	UFUNCTION()
	void OnRep_DungeonPhase(const FDungeonPhase& OldPhase);
	
	UPROPERTY(ReplicatedUsing=OnRep_ReadyPlayers)
	TArray<ASaiyoraPlayerCharacter*> ReadyPlayers;
	UFUNCTION()
	void OnRep_ReadyPlayers(const TArray<ASaiyoraPlayerCharacter*>& PreviousPlayers);
	UPROPERTY()
	TArray<ASaiyoraPlayerCharacter*> GroupPlayers;

	static const float CountdownLength;
	void StartCountdown();
	FTimerHandle CountdownHandle;
	void EndCountdown();
	void MoveToCompletedState();
	
	bool bInitialized = false;
	FDungeonRequirements DungeonRequirements;

	UPROPERTY(ReplicatedUsing=OnRep_DungeonProgress)
	FDungeonProgress DungeonProgress;
	UFUNCTION()
	void OnRep_DungeonProgress(const FDungeonProgress& PreviousProgress);
	void OnUpdatedProgress(const FDungeonProgress& PreviousProgress);
	FTimerHandle DepletionHandle;
	void DepleteDungeon();

	UPROPERTY()
	class ASaiyoraGameMode* GameModeRef;
};