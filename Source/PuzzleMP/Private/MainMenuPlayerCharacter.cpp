// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "MainMenuPlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AMainMenuPlayerCharacter::AMainMenuPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	RootComponent = Camera;
}

// Called when the game starts or when spawned
void AMainMenuPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SelectionLocations.Add(FTransform(FRotator(-6.200353, -73.595436, 0.000011), FVector(-202.182434, 2855.114258, 275.688934), FVector(1)));
	SelectionLocations.Add(FTransform(FRotator(-4.400212, -75.194229, 0.000013), FVector(712.255371, 2833.116211, 270.354919), FVector(1)));
	OnChangeActiveSelection(0);
}

// Called every frame
void AMainMenuPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(1 > CurrentAlpha) CurrentAlpha = FMath::Clamp(CurrentAlpha + (2.0 * DeltaTime), 0.0, 1.0);

	const FTransform NewTransform = UKismetMathLibrary::TLerp(this->GetTransform(), TargetTransform, CurrentAlpha);
	this->SetActorTransform(NewTransform);

	if(FMath::IsNearlyEqual(CurrentAlpha, 1, 0.4f)) ActiveSelection = PendingActiveSelection;
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
	int NewValue = FMath::Clamp(ActiveSelection - 1, 0, 1);
	if(NewValue == ActiveSelection) return;
	SetActiveSelection(NewValue);
}
void AMainMenuPlayerCharacter::NavigateRight()
{
	int NewValue = FMath::Clamp(ActiveSelection + 1, 0, 1);
	if(NewValue == ActiveSelection) return;
	SetActiveSelection(NewValue);
}
void AMainMenuPlayerCharacter::OnChangeActiveSelection(int NewSelection)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MainMenuPlayerCharacter: Changed selection"), true, true, FColor::Blue, 2);
	if(!SelectionLocations.IsValidIndex(NewSelection))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MainMenuPlayerCharacter: Invalid selection index"), true, true, FColor::Red, 2);
		return;
	}
	CurrentAlpha = 0;
	TargetTransform = SelectionLocations[NewSelection];
}
void AMainMenuPlayerCharacter::SetActiveSelection(int NewSelection)
{
	ActiveSelection = INDEX_NONE;
	PendingActiveSelection = NewSelection;
	OnChangeActiveSelection(NewSelection);
}

void AMainMenuPlayerCharacter::Select()
{
	if(ActiveSelection == INDEX_NONE) return;
	UE_LOG(LogTemp, Log, TEXT("Hit ENTER on index: %s"), *FString::SanitizeFloat(ActiveSelection));

	switch(ActiveSelection)
	{
	case 0:
		// TODO: Fade to black, TP both players to current puzzle, switch both to puzzle character, fade from black.
		break;
	case 1:
		UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
		break;
	default:
		break;
	}
}


