#include "SaiyoraGameMode.h"
#include "SaiyoraGAS/CoreClasses/SaiyoraGameState.h"
#include "SaiyoraGAS/CoreClasses/SaiyoraPlayerCharacter.h"
#include "SaiyoraGAS/CoreClasses/SaiyoraPlayerController.h"
#include "SaiyoraGAS/CoreClasses/SaiyoraPlayerState.h"

ASaiyoraGameMode::ASaiyoraGameMode()
{
	PlayerControllerClass = ASaiyoraPlayerController::StaticClass();
	DefaultPawnClass = ASaiyoraPlayerCharacter::StaticClass();
	GameStateClass = ASaiyoraGameState::StaticClass();
	PlayerStateClass = ASaiyoraPlayerState::StaticClass();
}
