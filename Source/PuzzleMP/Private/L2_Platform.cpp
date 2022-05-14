// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "L2_Platform.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AL2_Platform::AL2_Platform()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ClosedLocation = CreateDefaultSubobject<USceneComponent>(FName("Closed Location"));
	ClosedLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	NumberPanel = CreateDefaultSubobject<UStaticMeshComponent>(FName("Number Panel"));
	NumberPanel->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/General/SM_Display_Small.SM_Display_Small"));
	if(MeshAsset.Succeeded()) NumberPanel->SetStaticMesh(MeshAsset.Object);
}

UMaterialInstanceDynamic* AL2_Platform::GetNumberPanelMaterial()
{
	if(!NumberPanelMaterial)
		NumberPanelMaterial = NumberPanel->CreateDynamicMaterialInstance(1);
	return NumberPanelMaterial;
}


void AL2_Platform::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( AL2_Platform, TargetAlpha );
	DOREPLIFETIME( AL2_Platform, PlatformNumber );
}

// Called when the game starts or when spawned
void AL2_Platform::BeginPlay()
{
	Super::BeginPlay();

	GetNumberPanelMaterial()->SetScalarParameterValue(FName("Number"), PlatformNumber);
}

// Called every frame | Contains move logic
void AL2_Platform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Adjusts Alpha by x amount every frame until it matches TargetAlpha
	if(TargetAlpha > Alpha)
		Alpha = FMath::Clamp(Alpha + (DeltaTime * 1.6f), 0.0f, 1.0f); // Extend at 1.6x DeltaTime
	if(TargetAlpha < Alpha)
		Alpha = FMath::Clamp(Alpha - (DeltaTime * 1.8f), 0.0f, 1.0f); // Retract at 1.8x DeltaTime (faster than extending)

	// Set the Platform's mesh to the new location, using the Alpha plugged into a Lerp function (0 alpha = open | 1 alpha = closed)
	Mesh->SetWorldLocation(FMath::Lerp(this->GetActorLocation(), ClosedLocation->GetComponentLocation(),  Alpha));


	// Next section only executes if machine is server (host)
	if(HasAuthority()){
		if(Alpha == TargetAlpha) // If platform is stationary
		{
			if(!bDoneMoving) // If bDoneMoving bool is false, set it to true
			{
				bDoneMoving = true; // As this is now true, this logic won't get executed again until bDoneMoving is set back to false (acting like a Blueprints DoOnce switch)
				if(TargetAlpha == 1.0f && Alpha == TargetAlpha) // If platform is extended (closed)
				{
					UE_LOG(LogTemp, Log, TEXT("Done moving out")); // Platform has just closed

					// Waits 4.0 seconds before retracting the platform again
					FTimerHandle UnusedHandle;
					GetWorldTimerManager().SetTimer(UnusedHandle, [&]
					{
						UE_LOG(LogTemp, Log, TEXT("Retracting platform"));
						RetractPlatform();
					}, 0.1f, false, 4.0f);
				}
				if(TargetAlpha == 0.0f && Alpha == TargetAlpha) // If platform is retracted (open)
				{
					UE_LOG(LogTemp, Log, TEXT("Done moving in")); // Platform has just opened
					OnRetractDelegate.Broadcast(); // Broadcast via the delegate that this has just happened so other classes (ButtonGrid) know
				}
			}
		}
		if(Alpha != TargetAlpha) bDoneMoving = false; // Allows the loop to be reset once the platform starts moving again
	}
}


// Sets the TargetAlpha to 1, telling the tick function to begin moving the platform to the closed position | Can only be executed by server
void AL2_Platform::ExtendPlatform()
{
	if(!HasAuthority()) return;
	TargetAlpha = 1.0f;
}

// Sets the TargetAlpha to 0, telling the tick function to begin moving the platform back towards the open position | Can only be executed by server
void AL2_Platform::RetractPlatform()
{
	if(!HasAuthority()) return;
	TargetAlpha = 0.0f;
}


// Returns the PlatformNumber variable | Used by other classes as PlatformNumber is private
int AL2_Platform::GetPlatformNumber() { return PlatformNumber; }

// Changes the PlatformNumber to the new value | Can only be executed by server
void AL2_Platform::SetPlatformNumber(int NewNumber)
{
	if(!HasAuthority()) return;
	PlatformNumber = NewNumber; // Updates the PlatformNumber variable, which triggers the OnChangePlatformNumber function for all clients
	GetNumberPanelMaterial()->SetScalarParameterValue(FName("Number"), PlatformNumber); // Updates the ButtonPanel's material to reflect the new number
}

// Called automatically via replication whenever the PlatformNumber is changed by the server (host) machine
void AL2_Platform::OnChangePlatformNumber()
{
	// Updates the ButtonPanel's material to reflect the new number
	GetNumberPanelMaterial()->SetScalarParameterValue(FName("Number"), PlatformNumber);
}