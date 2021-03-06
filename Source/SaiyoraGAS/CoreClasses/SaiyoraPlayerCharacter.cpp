#include "SaiyoraPlayerCharacter.h"

#include "SaiyoraGameState.h"
#include "Camera/CameraComponent.h"
#include "Character/SaiyoraMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASaiyoraPlayerCharacter::ASaiyoraPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<USaiyoraMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	AbilityComponent = CreateDefaultSubobject<USaiyoraAbilityComponent>(TEXT("AbilityComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
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
	InitializeSaiyoraCharacter();
}

void ASaiyoraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeSaiyoraCharacter();
}

void ASaiyoraPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	InitializeSaiyoraCharacter();
}

void ASaiyoraPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SaiyoraGameStateRef = GetWorld()->GetGameState<ASaiyoraGameState>();
	InitializeSaiyoraCharacter();
}

void ASaiyoraPlayerCharacter::InitializeSaiyoraCharacter()
{
	if (bInitialized)
	{
		return;
	}
	if (!SaiyoraGameStateRef || !GetPlayerState() || (GetLocalRole() != ROLE_SimulatedProxy && !GetController()))
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