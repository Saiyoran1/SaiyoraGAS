#include "SaiyoraGameMode.h"
#include "OnlineSubsystemUtils.h"
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

void ASaiyoraGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	GameStateRef = GetGameState<ASaiyoraGameState>();
	GameStateRef->InitDungeonState();
}

void ASaiyoraGameMode::PreventFurtherJoining()
{
	if (const IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld()))
	{
		const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			if (const FNamedOnlineSession* CurrentGameSession = SessionInterface->GetNamedSession(NAME_GameSession))
			{
				FOnlineSessionSettings SessionSettings = CurrentGameSession->SessionSettings;
				SessionSettings.bAllowJoinInProgress = false;
				SessionInterface->UpdateSession(NAME_GameSession, SessionSettings);
			}
		}
	}
}
