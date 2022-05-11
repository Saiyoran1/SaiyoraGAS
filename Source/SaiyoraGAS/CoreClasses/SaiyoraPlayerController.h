#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SaiyoraPlayerController.generated.h"

UCLASS()
class SAIYORAGAS_API ASaiyoraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void AcknowledgePossession(APawn* P) override;
};
