#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "SaiyoraGAS/AbilitySystem/Components/HealthComponent.h"
#include "SaiyoraGAS/AbilitySystem/Components/SaiyoraAbilityComponent.h"
#include "SaiyoraPlayerCharacter.generated.h"

UCLASS()
class SAIYORAGAS_API ASaiyoraPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ASaiyoraPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Camera")
	class USpringArmComponent* GetSpringArm() { return SpringArm; }
	UFUNCTION(BlueprintPure, Category = "Camera")
	class UCameraComponent* GetCamera() { return Camera; }
	UFUNCTION(BlueprintPure, Category = "Abilities")
	USaiyoraAbilityComponent* GetAbilityComponent() { return AbilityComponent; }
	UFUNCTION(BlueprintPure, Category = "Health")
	UHealthComponent* GetHealthComponent() { return HealthComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilityComponent; }

	UFUNCTION(BlueprintPure)
	class ASaiyoraGameState* GetSaiyoraGameState() const { return SaiyoraGameStateRef; }

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void CreateUserInterface();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USaiyoraAbilityComponent* AbilityComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;
	
	UFUNCTION()
	void MoveCharacterForward(float Value);
	UFUNCTION()
	void MoveCharacterRight(float Value);
	UFUNCTION()
	void AbilityInput(FKey Key);
	UFUNCTION()
	void JumpInput();
	UFUNCTION()
	void ReloadInput();

	void InitializeSaiyoraCharacter();
	bool bInitialized = false;

	UPROPERTY()
	class ASaiyoraGameState* SaiyoraGameStateRef;
};
