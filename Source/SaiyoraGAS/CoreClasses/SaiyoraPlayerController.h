#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SaiyoraPlayerController.generated.h"

UCLASS()
class SAIYORAGAS_API ASaiyoraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* P) override;

private:

	UPROPERTY()
	class ASaiyoraGameState* GameStateRef;
	FTimerHandle PingHandle;
	FTimerDelegate PingDelegate;
	void SendPing();
	
	UFUNCTION(Server, Reliable)
	void ServerSendClientTime(const float ClientTime);
	UFUNCTION(Client, Reliable)
	void ClientSendServerTime(const float ServerTime, const float ClientRequestTime);
};
