#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SaiyoraPlayerCharacter.h"
#include "SaiyoraGameState.generated.h"

UENUM(BlueprintType)
enum class EDungeonState : uint8
{
	None,
	WaitingToStart,
	Countdown,
	InProgress,
	Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyPlayersChanged, const TArray<ASaiyoraPlayerCharacter*>&, ReadyPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeChanged, const float, Time);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDungeonStateChanged, const EDungeonState, DungeonState);

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

	void SetDungeonState(const EDungeonState NewState);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void MarkPlayerReady(ASaiyoraPlayerCharacter* Player);
	UPROPERTY(BlueprintAssignable)
	FOnReadyPlayersChanged OnReadyPlayersChanged;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EDungeonState GetDungeonState() const { return DungeonState; }
	UPROPERTY(BlueprintAssignable)
	FOnDungeonStateChanged OnDungeonStateChanged;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDungeonStartTime() const { return StartTime; }
	UPROPERTY(BlueprintAssignable)
	FOnTimeChanged OnStartTimeChanged;
	UPROPERTY(BlueprintAssignable)
	FOnTimeChanged OnGoalTimeChanged;

private:

	float WorldTime = 0.0f;

	static const float CountdownLength;

	UPROPERTY(ReplicatedUsing=OnRep_DungeonState)
	EDungeonState DungeonState = EDungeonState::None;
	UFUNCTION()
	void OnRep_DungeonState();
	
	UPROPERTY(ReplicatedUsing=OnRep_ReadyPlayers)
	TArray<ASaiyoraPlayerCharacter*> ReadyPlayers;
	UFUNCTION()
	void OnRep_ReadyPlayers();

	void StartCountdown();
	UPROPERTY(ReplicatedUsing=OnRep_StartTime)
	float StartTime = 0.0f;
	UFUNCTION()
	void OnRep_StartTime();
	FTimerHandle CountdownHandle;
	void EndCountdown();
	UPROPERTY(ReplicatedUsing=OnRep_GoalTime)
	float GoalTime = 0.0f;
	UFUNCTION()
	void OnRep_GoalTime();

	UPROPERTY()
	class ASaiyoraGameMode* GameModeRef;
};
