#pragma once
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SaiyoraMovementComponent.generated.h"

UCLASS()
class SAIYORAGAS_API USaiyoraMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;

public:

	UPROPERTY()
	class UHealthComponent* HealthComponentRef;
	UFUNCTION()
	void OnLifeStatusChanged(const bool bIsAlive);
};
