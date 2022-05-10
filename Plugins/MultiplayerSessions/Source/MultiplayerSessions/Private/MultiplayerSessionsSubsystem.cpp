#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

FSaiyoraSession::FSaiyoraSession()
{
	HostName = "";
	TotalSlots = 0;
	OpenSlots = 0;
	MatchType = "";
	bPrivate = false;
	ServerName = "";
	MapName = "";
}

FSaiyoraSession::FSaiyoraSession(const FOnlineSessionSearchResult& SearchResult)
{
	NativeResult = SearchResult;
	HostName = SearchResult.Session.OwningUserName;
	TotalSlots = SearchResult.Session.SessionSettings.NumPublicConnections;
	OpenSlots = SearchResult.Session.NumOpenPublicConnections;
	SearchResult.Session.SessionSettings.Get(FName(TEXT("MatchType")), MatchType);
	SearchResult.Session.SessionSettings.Get(FName(TEXT("ServerName")), ServerName);
	SearchResult.Session.SessionSettings.Get(FName(TEXT("MapName")), MapName);
	bPrivate = !SearchResult.Session.SessionSettings.bShouldAdvertise;
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSession)),
	FindSessionsDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessions)),
	JoinSessionDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSession)),
	DestroySessionDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySession))
	//StartSessionCallback(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSession))
{
	
}

IOnlineSessionPtr UMultiplayerSessionsSubsystem::GetSessionInterface()
{
	if (!SessionInterface.IsValid())
	{
		if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
		{
			SessionInterface = Subsystem->GetSessionInterface();
		}
	}
	return SessionInterface;
}

void UMultiplayerSessionsSubsystem::CreateSession(const bool bPrivate, const int32 NumPlayers, const FString& ServerName, const FString& MapName,
	const FString& MatchType, const FOnCreateSessionCallback& Callback)
{
	if (!Callback.IsBound())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid create session callback."));
		}
		return;
	}
	if (bCreatingSession)
	{
		Callback.Execute(false, TEXT("Currently creating a session, cannot create session again."));
		return;
	}
	if (bJoiningSession)
	{
		Callback.Execute(false, TEXT("Currently joining a session, cannot create session."));
		return;
	}
	if (bFindingSessions)
	{
		Callback.Execute(false, TEXT("Currently finding sessions, cannot create session."));
		return;
	}
	if (bDestroyingSession)
	{
		Callback.Execute(false, TEXT("Currently destroying session, cannot create session"));
		return;
	}
	if (!GetSessionInterface().IsValid())
	{
		Callback.Execute(false, TEXT("Invalid session interface, cannot create session."));
		return;
	}
	bCreatingSession = true;
	CreateSessionHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionDelegate);
	CreateSessionConnectionNumber = NumPlayers;
	bCreateSessionPrivate = bPrivate;
	//TODO: Limit server name length, validation?
	CreateSessionServerName = ServerName;
	CreateSessionMatchType = MatchType;
	CreateSessionMapName = MapName;
	CreateSessionCallback = Callback;
	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		bWaitingOnDestroyForRemake = true;
		FOnDestroySessionCallback PostDestroy;
		PostDestroy.BindDynamic(this, &ThisClass::PostDestroyCreateSession);
		DestroySession(PostDestroy);
	}
	else
	{
		InternalCreateSession();
	}
}

void UMultiplayerSessionsSubsystem::InternalCreateSession()
{
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSettings->NumPublicConnections = bCreateSessionPrivate ? 0 : CreateSessionConnectionNumber;
	LastSessionSettings->NumPrivateConnections = bCreateSessionPrivate ? CreateSessionConnectionNumber : 0;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = !bCreateSessionPrivate;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->BuildUniqueId = 1;
	if (CreateSessionMatchType != "")
	{
		LastSessionSettings->Set(FName(TEXT("MatchType")), CreateSessionMatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}
	if (CreateSessionServerName != "")
	{
		LastSessionSettings->Set(FName(TEXT("ServerName")), CreateSessionServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}
	if (CreateSessionMapName != "")
	{
		LastSessionSettings->Set(FName(TEXT("MapName")), CreateSessionMapName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}
	CreateSessionConnectionNumber = 0;
	CreateSessionMatchType = "";
	bCreateSessionPrivate = false;
	CreateSessionServerName = "";
	SessionInterface->CreateSession(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);
}

void UMultiplayerSessionsSubsystem::OnCreateSession(FName SessionName, bool bWasSuccessful)
{
	bCreatingSession = false;
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionHandle);
	}
	if (CreateSessionCallback.IsBound())
	{
		CreateSessionCallback.Execute(bWasSuccessful, bWasSuccessful ? TEXT("Successfully created a session.") : TEXT("Session interface returned false when creating session."));
		CreateSessionCallback.Clear();
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(const int32 MaxSearchResults, const FOnFindSessionsCallback& Callback)
{
	if (!Callback.IsBound())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid find sessions callback."));
		}
		return;
	}
	if (MaxSearchResults <= 0)
	{
		Callback.Execute(false, TEXT("Max search results at or below 0, cannot find sessions."), TArray<FSaiyoraSession>());
		return;
	}
	if (bCreatingSession)
	{
		Callback.Execute(false, TEXT("Currently creating session, cannot find sessions."), TArray<FSaiyoraSession>());
		return;
	}
	if (bJoiningSession)
	{
		Callback.Execute(false, TEXT("Currently joining session, cannot find sessions."), TArray<FSaiyoraSession>());
		return;
	}
	if (bFindingSessions)
	{
		Callback.Execute(false, TEXT("Currently finding sessions, cannot find sessions again."), TArray<FSaiyoraSession>());
		return;
	}
	if (bDestroyingSession)
	{
		Callback.Execute(false, TEXT("Currently destroying session, cannot find sessions."), TArray<FSaiyoraSession>());
		return;
	}
	if (!GetSessionInterface().IsValid())
	{
		Callback.Execute(false, TEXT("Invalid session interface, cannot find sessions."), TArray<FSaiyoraSession>());
		return;
	}
	bFindingSessions = true;
	FindSessionsCallback = Callback;
	FindSessionsHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsDelegate);
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnFindSessions(bool bWasSuccessful)
{
	bFindingSessions = false;
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsHandle);
	}
	if (FindSessionsCallback.IsBound())
	{
		TArray<FSaiyoraSession> SessionResults;
		if (bWasSuccessful)
		{
			for (const FOnlineSessionSearchResult& Result : LastSessionSearch->SearchResults)
			{
				if (Result.IsValid())
				{
					SessionResults.Add(FSaiyoraSession(Result));
				}
			}
		}
		FindSessionsCallback.Execute(bWasSuccessful, bWasSuccessful ? TEXT("Successfully found sessions.") : TEXT("Session interface returned false when finding sessions."), SessionResults);
		FindSessionsCallback.Clear();
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FSaiyoraSession& Session, const FOnJoinSessionCallback& Callback)
{
	if (!Callback.IsBound())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid join session callback."));
		}
		return;
	}
	if (bCreatingSession)
	{
		Callback.Execute(false, TEXT("Currently creating session, cannot join session."));
		return;
	}
	if (bJoiningSession)
	{
		Callback.Execute(false, TEXT("Currently joining session, cannot join session again."));
		return;
	}
	if (bFindingSessions)
	{
		Callback.Execute(false, TEXT("Currently finding sessions, cannot join session."));
		return;
	}
	if (bDestroyingSession)
	{
		Callback.Execute(false, TEXT("Currently destroying session, cannot join session."));
		return;
	}
	if (!GetSessionInterface().IsValid())
	{
		Callback.Execute(false, TEXT("Invalid session interface, cannot join session."));
		return;
	}
	if (!Session.NativeResult.IsValid())
	{
		Callback.Execute(false, TEXT("Invalid session result, cannot join session."));
		return;
	}
	bJoiningSession = true;
	JoinSessionCallback = Callback;
	JoinSessionHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegate);
	SessionInterface->JoinSession(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), NAME_GameSession, Session.NativeResult);
}

void UMultiplayerSessionsSubsystem::OnJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	bJoiningSession = false;
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionHandle);
	}
	const bool bSuccess = Result == EOnJoinSessionCompleteResult::Success;
	if (JoinSessionCallback.IsBound())
	{
		JoinSessionCallback.Execute(bSuccess, bSuccess ? TEXT("Successfully joined session.") : TEXT("Session interface returned false when joining session."));
		JoinSessionCallback.Clear();
	}
	if (bSuccess)
	{
		FString ConnectString;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString);
		GetWorld()->GetFirstPlayerController()->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession(const FOnDestroySessionCallback& Callback)
{
	if (!Callback.IsBound())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("Invalid callback for destroy session."));
		}
		return;
	}
	if (bCreatingSession && !bWaitingOnDestroyForRemake)
	{
		Callback.Execute(false, TEXT("Currently creating session (not waiting on destroy), cannot destroy session."));
		return;
	}
	if (bJoiningSession)
	{
		Callback.Execute(false, TEXT("Currently joining session, cannot destroy session."));
		return;
	}
	if (bFindingSessions)
	{
		Callback.Execute(false, TEXT("Currently finding sessions, cannot destroy session."));
		return;
	}
	if (bDestroyingSession)
	{
		Callback.Execute(false, TEXT("Currently destroying session, cannot destroy session again."));
		return;
	}
	if (!GetSessionInterface().IsValid())
	{
		Callback.Execute(false, TEXT("Invalid session interface, cannot destroy session."));
		return;
	}
	bDestroyingSession = true;
	DestroySessionCallback = Callback;
	DestroySessionHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionDelegate);
	SessionInterface->DestroySession(NAME_GameSession);
}

void UMultiplayerSessionsSubsystem::OnDestroySession(FName SessionName, bool bWasSuccessful)
{
	bDestroyingSession = false;
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionHandle);
	}
	if (DestroySessionCallback.IsBound())
	{
		DestroySessionCallback.Execute(bWasSuccessful, bWasSuccessful ? TEXT("Successfully destroyed session.") : TEXT("Session interface returned false from destroying session."));
		DestroySessionCallback.Clear();
	}
}

void UMultiplayerSessionsSubsystem::PostDestroyCreateSession(const bool bWasSuccessful, const FString& Error)
{
	if (bWaitingOnDestroyForRemake)
	{
		bWaitingOnDestroyForRemake = false;
		if (bWasSuccessful)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Destroyed current session to create new session."));
			}
			InternalCreateSession();
		}
		else
		{
			bCreatingSession = false;
			if (CreateSessionCallback.IsBound())
			{
				CreateSessionCallback.Execute(false, TEXT("Session interface returned false from destroying existing session, cannot create session."));
				CreateSessionCallback.Clear();
			}
		}
	}
}


/*void UMultiplayerSessionsSubsystem::StartSession()
{
	
}*/

/*void UMultiplayerSessionsSubsystem::OnStartSession(FName SessionName, bool bWasSuccessful)
{
	
}*/
