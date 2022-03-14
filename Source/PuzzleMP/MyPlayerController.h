// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEMP_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
private:
	void OnTalkKeyPressed();
	void OnTalkKeyReleased();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool IsTalking = false;

	UPROPERTY(BlueprintReadWrite, Category=Default)
	FString PuzzleLevelName = "Puzzles_PersLevel";
};
