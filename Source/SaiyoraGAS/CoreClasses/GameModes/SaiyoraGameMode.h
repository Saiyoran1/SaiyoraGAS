#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameMode.h"
#include "SaiyoraGameMode.generated.h"

UCLASS()
class SAIYORAGAS_API ASaiyoraGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	ASaiyoraGameMode();
	virtual void HandleMatchHasStarted() override;
	void PreventFurtherJoining();

protected:
	
	UPROPERTY(EditDefaultsOnly)
	int32 KillCountRequirement = 0;
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Boss"))
	TSet<FGameplayTag> BossKillTags;

private:

	UPROPERTY()
	class ASaiyoraGameState* GameStateRef;
	UPROPERTY()
	TArray<class ASaiyoraPlayerCharacter*> PlayersReadyToStart;
};
