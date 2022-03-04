// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Engine/GameInstance.h"
#include "PuzzleMPGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEMP_API UPuzzleMPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	public:
		UPuzzleMPGameInstance();

	protected:
		TSharedPtr<FOnlineSessionSearch> SessionSearch;

		IOnlineSessionPtr SessionInterface;

		virtual void Init() override;
		virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
		virtual void OnFindSessionComplete(bool Succeeded);
		virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);


		//Blueprint callable events, used from UI.
		UFUNCTION(BlueprintCallable)
			void CreateServer();
		UFUNCTION(BlueprintCallable)
			void JoinServer();

};
