// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzleMPGameInstance.h"
#include "Kismet/GameplayStatics.h" //Used to access player controller
#include "Engine/World.h"

UPuzzleMPGameInstance::UPuzzleMPGameInstance() {

}

void UPuzzleMPGameInstance::Init() {
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) { //If it grabs a subsystem
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzleMPGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzleMPGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzleMPGameInstance::OnJoinSessionComplete);
		}
	}
}

void UPuzzleMPGameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateServer"))
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL"); //If no subsystem is present, use LAN, otherwise don't.
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 2;

	SessionInterface->CreateSession(0, FName("Puzzle Game Session"), SessionSettings);
}

void UPuzzleMPGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	if(!Succeeded) return;
	GetWorld()->ServerTravel("/Game/Maps/Puzzle01?listen");
}

void UPuzzleMPGameInstance::OnFindSessionComplete(bool Succeeded)
{
	if(!Succeeded) return;
	TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
	if(SearchResults.Num()) //If it contains no results (0), won't execute.
	{
		SessionInterface->JoinSession(0, FName("Puzzle Game Session"), SearchResults[0]); //Joins the first result. TODO: Remove this
	}
}

void UPuzzleMPGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress); //Sets the join address from the session name and stores it in JoinAddress
		if(JoinAddress != "") //If JoinAddress hasn't been set to anything, don't continue.
		{
			PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}



void UPuzzleMPGameInstance::JoinServer()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000; //High number because we're using the public Steam App ID so there'll potentially be a lot of sessions.
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals); //The server's 'SEARCH_PRESENCE' setting must be equal to true.

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
