// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"

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

void AMyPlayerController::BeginPlay()
{
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
	UE_LOG(LogTemp, Log, TEXT("Set up input mode"));
}



void AMyPlayerController::OnTalkKeyPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Started speaking"));
	ToggleSpeaking(true);
	IsTalking = true;
}

void AMyPlayerController::OnTalkKeyReleased()
{
	UE_LOG(LogTemp, Log, TEXT("Stopped speaking"));
	ToggleSpeaking(false);
	IsTalking = false;
}


