#include "SaiyoraPlayerCharacter.h"

#include "SaiyoraGameState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASaiyoraPlayerCharacter::ASaiyoraPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	AbilityComponent = CreateDefaultSubobject<USaiyoraAbilityComponent>(TEXT("AbilityComponent"));
}

void ASaiyoraPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("LookHorizontal", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookVertical", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASaiyoraPlayerCharacter::MoveCharacterForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASaiyoraPlayerCharacter::MoveCharacterRight);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ASaiyoraPlayerCharacter::JumpInput);
	PlayerInputComponent->BindAction("Ability", EInputEvent::IE_Pressed, this, &ASaiyoraPlayerCharacter::AbilityInput);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ASaiyoraPlayerCharacter::ReloadInput);
}

void ASaiyoraPlayerCharacter::PossessedBy(AController* NewController)
{
	//Only called on server.
	Super::PossessedBy(NewController);
	if (AbilityComponent)
	{
		AbilityComponent->InitAbilityActorInfo(this, this);
	}
	SetOwner(NewController);
	if (SaiyoraGameStateRef)
	{
		FinalInit();
	}
}

void ASaiyoraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (GetController() && SaiyoraGameStateRef)
		{
			FinalInit();
		}
	}
	else if (SaiyoraGameStateRef)
	{
		FinalInit();
	}
}

void ASaiyoraPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	if (!SaiyoraGameStateRef)
	{
		SaiyoraGameStateRef = GetWorld()->GetGameState<ASaiyoraGameState>();
		//TODO: Check/ensure?
	}
	if (GetPlayerState())
	{
		FinalInit();
		SaiyoraGameStateRef->InitPlayer(this);
	}
}

void ASaiyoraPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SaiyoraGameStateRef = GetWorld()->GetGameState<ASaiyoraGameState>();
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (SaiyoraGameStateRef && GetController() && GetPlayerState())
		{
			FinalInit();
		}
	}
	else if (SaiyoraGameStateRef && GetPlayerState())
	{
		FinalInit();
	}
}

void ASaiyoraPlayerCharacter::FinalInit()
{
	if (bInitialized)
	{
		return;
	}
	bInitialized = true;
	if (IsLocallyControlled())
	{
		CreateUserInterface();
	}
	SaiyoraGameStateRef->InitPlayer(this);
}

void ASaiyoraPlayerCharacter::MoveCharacterForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASaiyoraPlayerCharacter::MoveCharacterRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASaiyoraPlayerCharacter::AbilityInput(FKey Key)
{
	//TODO: Get keybind mapping, call ability input?
}

void ASaiyoraPlayerCharacter::JumpInput()
{
	//TODO: Jump ability.
}

void ASaiyoraPlayerCharacter::ReloadInput()
{
	//TODO: Reload ability.
}