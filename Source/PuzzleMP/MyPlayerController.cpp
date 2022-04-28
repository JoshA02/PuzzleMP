// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "PlayerHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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

void AMyPlayerController::FadeHUDToBlack()
{
	if(!IsLocalController()) return;
	APlayerHUD* HUD = Cast<APlayerHUD>(GetHUD()); // Get reference to the player's HUD class
	HUD->FadeToBlack();
}

void AMyPlayerController::Owner_FadeHUDToBlack_Implementation()
{
	if(!IsLocalController()) return;
	FadeHUDToBlack();
}
void AMyPlayerController::Owner_FadeHUDFromBlack_Implementation()
{
	if(!IsLocalController()) return;
	FadeHUDFromBlack();
}

void AMyPlayerController::FadeHUDFromBlack()
{
	if(!IsLocalController()) return;
	APlayerHUD* HUD = Cast<APlayerHUD>(GetHUD()); // Get reference to the player's HUD class
	HUD->FadeFromBlack();
}




