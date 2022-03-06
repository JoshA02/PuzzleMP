// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{
	// UE_LOG(LogTemp, Warning, TEXT("Test Player Controller"));
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &AMyPlayerController::OnTalkKeyPressed);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &AMyPlayerController::OnTalkKeyReleased);
}


void AMyPlayerController::OnTalkKeyPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Started speaking"));
	ToggleSpeaking(true);
}

void AMyPlayerController::OnTalkKeyReleased()
{
	UE_LOG(LogTemp, Log, TEXT("Stopped speaking"));
	ToggleSpeaking(false);
}


