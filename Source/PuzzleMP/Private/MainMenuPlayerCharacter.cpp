// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "MainMenuPlayerCharacter.h"

#include "PlayerHUD.h"
#include "SavingUtils.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PuzzleMP/MyCharacter.h"
#include "PuzzleMP/MyPlayerController.h"

// Sets default values
AMainMenuPlayerCharacter::AMainMenuPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	RootComponent = Camera;

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/Audio/Wavs/sfx_1p_playerjoin.sfx_1p_playerjoin"));
	if(SoundAsset.Succeeded()) JoinSound = SoundAsset.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> NoPlayerSoundAsset(TEXT("/Game/Audio/Wavs/sfx_1p_cantstart.sfx_1p_cantstart"));
	if(NoPlayerSoundAsset.Succeeded()) CantStartSound = NoPlayerSoundAsset.Object;
}

void AMainMenuPlayerCharacter::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( AMainMenuPlayerCharacter, CanStart ); // Sets up replication for CanStart bool
}

// Called when the game starts or when spawned
void AMainMenuPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> TempPanels;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoorPanel::StaticClass(), TempPanels); // Gets all the ADoorPanel actors and stores them to TempPanels array
	for(AActor* PanelActor : TempPanels) // Loop through all the ADoorPanel actors in the world
	{
		ADoorPanel* DoorPanel = Cast<ADoorPanel>(PanelActor); // Cast to ensure the actor is a door panel
		if(!DoorPanel) continue; // If not, skip this loop iteration
		DoorPanels.Insert(DoorPanel, DoorPanel->PanelIndex); // Inserts the door panel into the array, using the PanelIndex as the position
	}
	
	SelectionLocations.Add(FTransform(FRotator(-6.200353, -73.595436, 0.000011), FVector(-202.182434, 2855.114258, 275.688934), FVector(1)));
	SelectionLocations.Add(FTransform(FRotator(-4.400212, -75.194229, 0.000013), FVector(712.255371, 2833.116211, 270.354919), FVector(1)));
	OnChangeActiveSelection(0); // Set "begin" door as initial door to look at
}

// Called every frame
void AMainMenuPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		const bool NewValue = UGameplayStatics::GetGameState(GetWorld())->PlayerArray.Num() > 1;
		if(CanStart != NewValue)
		{
			CanStart = NewValue;
			if(CanStart) UGameplayStatics::PlaySound2D(GetWorld(), JoinSound);
		}
	}

	if(DoorPanels.Num() < 2) return; // Have DoorPanels[0] and [1] been defined? If not, stop here.
	
	if(CanStart) // 2 players in lobby
		DoorPanels[0]->SetSubText(DoorPanelStates::DS_EnterToStart, DoorPanelTextColours::TC_Subheading); // Updates "Begin" panel to reflect being able to start.
	if(!CanStart) // 1 player in lobby
		DoorPanels[0]->SetSubText(DoorPanelStates::DS_2PlayersNeeded, DoorPanelTextColours::TC_Alert); // Updates "Begin" panel to reflect not being able to start.
	
	
	if(1 > CurrentAlpha) CurrentAlpha = FMath::Clamp(CurrentAlpha + (2.0 * DeltaTime), 0.0, 1.0);

	const FTransform NewTransform = UKismetMathLibrary::TLerp(this->GetTransform(), TargetTransform, CurrentAlpha);
	this->SetActorTransform(NewTransform);

	if(FMath::IsNearlyEqual(CurrentAlpha, 1, 0.4f)) ActiveSelection = PendingActiveSelection; // Sets the ActiveSelection to the pending selection, but only once the camera has almost reached the new location (IsNearlyEqual)
	
}

// Called to bind functionality to input
void AMainMenuPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("NavigateLeft", IE_Pressed, this, &AMainMenuPlayerCharacter::NavigateLeft);
	PlayerInputComponent->BindAction("NavigateRight", IE_Pressed, this, &AMainMenuPlayerCharacter::NavigateRight);
	PlayerInputComponent->BindAction("MenuSelect", IE_Pressed, this, &AMainMenuPlayerCharacter::Select);
}

void AMainMenuPlayerCharacter::NavigateLeft()
{
	if(ActiveSelection == INDEX_NONE) return; // If there's no active selection (if the camera is currently moving from A to B), stop here
	int NewValue = FMath::Clamp(ActiveSelection - 1, 0, 1); // Get the new selection (if they're at door 1, set to door 0)
	if(NewValue == ActiveSelection) return; // If the player is already at that door, stop here
	SetActiveSelection(NewValue); // Otherwise, set the new selection using the new value (0).
}
void AMainMenuPlayerCharacter::NavigateRight()
{
	if(ActiveSelection == INDEX_NONE) return; // If there's no active selection (if the camera is currently moving from A to B), stop here
	int NewValue = FMath::Clamp(ActiveSelection + 1, 0, 1); // Get the new selection (if they're at door 0, set to door 1)
	if(NewValue == ActiveSelection) return; // If the player is already at that door, stop here
	SetActiveSelection(NewValue); // Otherwise, set the new selection using the new value (1).
}
void AMainMenuPlayerCharacter::OnChangeActiveSelection(int NewSelection)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MainMenuPlayerCharacter: Changed selection"), true, true, FColor::Blue, 2);
	if(!SelectionLocations.IsValidIndex(NewSelection)) // Ensures the NewSelection is a valid selection (0 or 1)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MainMenuPlayerCharacter: Invalid selection index"), true, true, FColor::Red, 2);
		return;
	}
	CurrentAlpha = 0; // Resets the CurrentAlpha to 0, allowing the Tick function to start lerping from current location to new location
	TargetTransform = SelectionLocations[NewSelection]; // Tells the Tick function what the new location is (TargetTransform)
}
void AMainMenuPlayerCharacter::SetActiveSelection(int NewSelection)
{
	ActiveSelection = INDEX_NONE; // Temporarily blocks any player input
	PendingActiveSelection = NewSelection; // Tells the class which selection it wants to move to
	OnChangeActiveSelection(NewSelection);
}

void AMainMenuPlayerCharacter::Select()
{
	if(!CanSelect) return;
	if(ActiveSelection == INDEX_NONE) return;
	UE_LOG(LogTemp, Log, TEXT("Hit ENTER on index: %s"), *FString::SanitizeFloat(ActiveSelection));

	switch(ActiveSelection)
	{
	case 0: // If hitting enter on "begin" door
		if(!HasAuthority()) return;
		if(!CanStart) UGameplayStatics::PlaySound2D(GetWorld(), CantStartSound, 0.4);
		if(CanStart)
		{
			CanSelect = false; // Stops user from pressing ENTER whilst transitioning to new level
			
			LevelNumberToLoad = SavingUtils::GetSavedLevel();
			UE_LOG(LogTemp, Log, TEXT("Level: %i"), LevelNumberToLoad);
			if(SavingUtils::GetSpawnLocations().Num() < 2) return; // Make sure there's two spawn points available

			// Loops through all players (0 and 1)
			AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
			for(int x = 0; x < GameState->PlayerArray.Num(); x++)
			{
				if(x < 0 || x > 1)
				{
					UKismetSystemLibrary::PrintString(GetWorld(), TEXT("More than 2 players present! Skipping player"), true, true, FColor::Red, 2);
					continue;
				}
					
				const APlayerState* CurrentPlayerState = GameState->PlayerArray[x]; // Get reference to the player's PlayerState
				AMyPlayerController* CurrentPlayerController = Cast<AMyPlayerController>(CurrentPlayerState->GetOwner()); // Get reference to the player's PlayerController
					
				CurrentPlayerController->Owner_FadeHUDToBlack(); // Fade the player's screen to black
			}
				
			// Start a timer which, after some seconds, spawns the new player character in the puzzle, possesses it, and fades from black
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMainMenuPlayerCharacter::SpawnNewCharactersAndPossess, 3, false);
			break;
		}
		break;
	case 1: // If hitting enter on "Quit" door
		UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
		break;
	default:
		break;
	}
}

// Spawns the new player characters in the puzzle area and fades each player from black
void AMainMenuPlayerCharacter::SpawnNewCharactersAndPossess()
{
	TArray<FTransform> SpawnLocations = SavingUtils::GetSpawnLocations();
	if(SpawnLocations.Num() < 2) return; // Make sure there's two spawn points available

	const FActorSpawnParameters SpawnParams;
	TArray<AMyCharacter*> NewPlayerCharacters;
	NewPlayerCharacters.Add(Cast<AMyCharacter>(GetWorld()->SpawnActor(GameplayCharacterClass, &SpawnLocations[0], SpawnParams)));
	NewPlayerCharacters.Add(Cast<AMyCharacter>(GetWorld()->SpawnActor(GameplayCharacterClass, &SpawnLocations[1], SpawnParams)));
	
	// Loop through all players (0 and 1) via their PlayerStates
	AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
	for(int x = 0; x < GameState->PlayerArray.Num(); x++)
	{
		const APlayerState* CurrentPlayerState = GameState->PlayerArray[x]; // Get reference to the player's PlayerState
		AMyPlayerController* CurrentPlayerController = Cast<AMyPlayerController>(CurrentPlayerState->GetOwner()); // Get reference to the player's PlayerController
		APawn* CurrentPlayerPawn = CurrentPlayerController->GetPawn();

		if(!CurrentPlayerController) return;
		if(!NewPlayerCharacters.IsValidIndex(x)) return;
		
		CurrentPlayerController->Possess(NewPlayerCharacters[x]);
		CurrentPlayerController->Owner_FadeHUDFromBlack();
		CurrentPlayerPawn->Destroy();
	}
}
