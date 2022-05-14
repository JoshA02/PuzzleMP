// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "ButtonGrid.h"

#include "Kismet/KismetSystemLibrary.h"
#include "PuzzleMP/Trigger.h"

// Sets default values
AButtonGrid::AButtonGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Setup the desired TriggerPositions for each button (5)
	TriggerPositions.Empty();
	TriggerPositions.Add(FVector(0, -10, 60));
	TriggerPositions.Add(FVector(0, 10, 60));
	TriggerPositions.Add(FVector(0, -10, 40));
	TriggerPositions.Add(FVector(0, 10, 40));
	TriggerPositions.Add(FVector(0, 0, 19));

	// Sets up the BodyMesh component and sets it to the SM_ButtonGrid mesh
	UStaticMeshComponent* BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Body"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/L2/SM_ButtonGrid.SM_ButtonGrid"));
	if(MeshAsset.Succeeded()) BodyMesh->SetStaticMesh(MeshAsset.Object);

	// Sets up the ButtonMaterials array
	SetupButtonMaterials();

	// Sets the BodyMesh as the root component of the actor
	RootComponent = BodyMesh;
}

void AButtonGrid::SetupButtonMaterials()
{
	UStaticMeshComponent* BodyMesh = Cast<UStaticMeshComponent>(RootComponent);
	if(!BodyMesh) return;
	if(ButtonMaterials.Num() > 0) return;
	for(int x = 1; x < 6; x++)
	{
		// Sets up a new material instance for each of the BodyMesh's button material slots (slots 1 - 5; slot 0 is the body material)
		UMaterialInstanceDynamic* Material = BodyMesh->CreateDynamicMaterialInstance(x);
		Material->SetScalarParameterValue("Number", x);
		ButtonMaterials.Add(Material);
	}
}


// Called when the game starts or when spawned
void AButtonGrid::BeginPlay()
{
	Super::BeginPlay();
	SetupButtonMaterials();

	if(!HasAuthority()) return;

	// Spawns a trigger at each of the TriggerPositions, but only for the host machine, as it's the host that handles interacting
	for(FVector TriggerPosition : TriggerPositions)
	{
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(TriggerPosition + GetActorLocation(), FRotator(0));
		Triggers.Add(Trigger);
		Trigger->OnTriggerDelegate.AddDynamic(this, &AButtonGrid::OnButtonPressed);
	}

	// Starts the game by randomising the numbers of each of the platforms it's controlling
	RandomisePlatformNumbers();
}

void AButtonGrid::RandomisePlatformNumbers()
{
	if(!HasAuthority()) return;
	TArray<int> Numbers; // This array contains a series of numbers (1, 2, 3, 4, 5)
	for(int x = 1; x < Platforms.Num() + 1; x++)
		Numbers.Add(x);

	// Goes through each platform, setting its number to a random one from the array, before removing it from the array so it cannot be repeated
	for(int x = 0; x < Platforms.Num(); x++)
	{
		int RandIndex = FMath::RandRange(0, Numbers.Num() - 1);
		Platforms[x]->SetPlatformNumber(Numbers[RandIndex]);
		Numbers.RemoveAt(RandIndex);
	}
}


// Executed automatically when one of the buttons is pressed (interacted with)
void AButtonGrid::OnButtonPressed(AActor* TriggeringActor, AActor* TriggerActor)
{
	if(!HasAuthority()) return;
	ATrigger* Trigger = Cast<ATrigger>(TriggerActor);
	if(!Triggers.Contains(Trigger)) // Makes sure the trigger is in the Triggers array (defined earlier) before continuing
	{
		UE_LOG(LogTemp, Error, TEXT("Trigger not in Triggers array... somehow"));
		return;
	}
	
	const int Index = Triggers.Find(Trigger); // Grabs the index of the trigger (button 1 = index of 0, button 2 = index of 1, etc) within the Triggers array
	const int Number = Index + 1; // From here, gets the platform number to target by just adding one (button 0 targets platform 1, button 1 targets platform 2, etc)

	// Sets the state of the button to true, meaning it'll update the button material to appear lit | does this to both server and client players
	SetButtonState(Index, true);
	SetButtonStateMulticast(Index, true);
	
	UKismetSystemLibrary::PrintString(GetWorld(), (TEXT("%s"), *FString::SanitizeFloat(Number)), true, true, FColor::Blue, 2);

	// Gets the platform by the number stated earlier. This is important as the platforms' numbers change after each sequence
	AL2_Platform* Platform = GetPlatformByNumber(Number);
	if(PendingPlatforms.Contains(Platform)) // If the button for this platform has already been pressed during this sequence, ignore it
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Platform already pending for this sequence"), true, true, FColor::Orange, 2);
		return;
	}
	PendingPlatforms.Add(Platform); // Add the pressed button's platform to the PendingPlatforms array, queueing it up to be pushed out soon
	Platform->OnRetractDelegate.AddDynamic(this, &AButtonGrid::OnPlatformMoveComplete); // Bind the platform's OnRetractDelegate to the OnPlatformMoveComplete function, so it knows when the platform has retracted again
	PlatformsPending ++; // Remembers how many platforms are pending (used independently from PendingPlatforms.Num())
	
	if(PendingPlatforms.Num() >= 5) // If the button just pressed was the fifth button to be pressed, start the sequence
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Start sequence"), true, true, FColor::Blue, 2);
		StartSequence();
	}
}

// Pushes each platform out, in sequence order
void AButtonGrid::StartSequence()
{
	SequenceIndex = 0; // Starts at the first platform in the PendingPlatforms array
	GetWorldTimerManager().SetTimer(SequenceHandle, [&]
	{
		UE_LOG(LogTemp, Log, TEXT("INDEX TO PUSH OUT: %s"), *FString::SanitizeFloat(SequenceIndex));
		AL2_Platform* Platform = PendingPlatforms[SequenceIndex];
		Platform->ExtendPlatform(); // Pushes the platform out, which automatically gets retracted after a few seconds (by the L2_Platform class)
		SequenceIndex ++; // Increment SequenceIndex so the next platform in the array gets targeted
		if(SequenceIndex > PendingPlatforms.Num() - 1) GetWorldTimerManager().ClearTimer(SequenceHandle); // If the last platform was just pushed out, cancel the timer so no more are pushed
	}, 1.0f, true, 0.0f);
}

void AButtonGrid::SetButtonStateMulticast_Implementation(int ButtonIndex, bool bNewState)
{
	SetButtonState(ButtonIndex, bNewState);
}
void AButtonGrid::SetButtonState(int ButtonIndex, bool bNewState)
{
	if(!ButtonMaterials[ButtonIndex])
	SetupButtonMaterials();

	ButtonMaterials[ButtonIndex]->SetScalarParameterValue(FName("State"), bNewState);
}

// Returns the platform that matches the provided number
AL2_Platform* AButtonGrid::GetPlatformByNumber(int Number)
{
	for(AL2_Platform* Platform : Platforms)
		if(Platform->GetPlatformNumber() == Number) return Platform;
	
	return nullptr;
}


/* Once a platform has extended, and retracted, this function automatically gets called.
 * Here, the PlatformsPending counter gets decremented by 1. This helps keep track of how many of the platforms have finished moving
*/
void AButtonGrid::OnPlatformMoveComplete() { PlatformsPending = FMath::Clamp(PlatformsPending - 1, 0, 999); }

void AButtonGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Once PlatformsPending is 0, meaning all the platforms have extended and retracted...
	if(PendingPlatforms.Num() >= 5 && PlatformsPending == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("All platforms done moving"));
		RandomisePlatformNumbers(); // Randomises each platform's number
		for(AL2_Platform* Platform : PendingPlatforms)
			Platform->OnRetractDelegate.Clear(); // Clear the delegate so it doesn't get executed multiple times when bound again later
		PendingPlatforms.Empty(); // Forget the PendingPlatforms, allowing the buttons to be pressed again.
		for(int x = 0; x < ButtonMaterials.Num(); x++)
		{
			SetButtonState(x, false); // Set the buttons back to their default state visually
			SetButtonStateMulticast(x, false);
		}
	}
}
