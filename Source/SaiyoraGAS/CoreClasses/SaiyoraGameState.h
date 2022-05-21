#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SaiyoraPlayerCharacter.h"
#include "SaiyoraGameState.generated.h"

USTRUCT(BlueprintType)
struct FDungeonInfo
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyPlayersChanged, const TArray<ASaiyoraPlayerCharacter*>&, ReadyPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, const float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDungeonStateChanged, const FDungeonPhase&, OldState, const FDungeonPhase&, NewState);

UCLASS()
class SAIYORAGAS_API ASaiyoraGameState : public AGameState
{
	GENERATED_BODY()

public:

	ASaiyoraGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float GetServerWorldTimeSeconds() const override;
	void ReportAdjustedServerTime(const float AdjustedTime);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void MarkPlayerReady(ASaiyoraPlayerCharacter* Player);
	UPROPERTY(BlueprintAssignable)
	FOnReadyPlayersChanged OnReadyPlayersChanged;

	void InitDungeonState();
	UPROPERTY(BlueprintAssignable)
	FOnDungeonStateChanged OnDungeonStateChanged;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EDungeonState GetDungeonState() const { return DungeonPhase.DungeonState; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDungeonStartTime() const { return DungeonPhase.PhaseStartTime; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDungeonEndTime() const { return DungeonPhase.PhaseEndTime; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDungeonTimeLimit() const { return DungeonInfo.TimeLimit; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetDungeonKillCount() const { return DungeonInfo.KillCountRequirement; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetBossKillRequirements(FGameplayTagContainer& OutTags) const { OutTags = DungeonInfo.BossKillTags; }

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FDungeonInfo> DungeonInfoTable;

private:

	float WorldTime = 0.0f;

	UPROPERTY(ReplicatedUsing=OnRep_DungeonPhase)
	FDungeonPhase DungeonPhase;
	UFUNCTION()
	void OnRep_DungeonPhase(const FDungeonPhase& OldPhase);
	
	UPROPERTY(ReplicatedUsing=OnRep_ReadyPlayers)
	TArray<ASaiyoraPlayerCharacter*> ReadyPlayers;
	UFUNCTION()
	void OnRep_ReadyPlayers();

	static const float CountdownLength;
	void StartCountdown();
	FTimerHandle CountdownHandle;
	void EndCountdown();

	FDungeonInfo DungeonInfo;

	UPROPERTY()
	class ASaiyoraGameMode* GameModeRef;
};