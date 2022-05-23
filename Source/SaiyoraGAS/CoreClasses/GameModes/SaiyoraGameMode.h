#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SaiyoraGameMode.generated.h"

UCLASS()
class SAIYORAGAS_API ASaiyoraGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	ASaiyoraGameMode();
	virtual void HandleMatchHasStarted() override;

private:

	UPROPERTY()
	class ASaiyoraGameState* GameStateRef;
	UPROPERTY()
	TArray<class ASaiyoraPlayerCharacter*> PlayersReadyToStart;
};
