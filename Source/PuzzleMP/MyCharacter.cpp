// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "DrawDebugHelpers.h"
#include "Trigger.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/VoiceConfig.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;

	//Set up turn rate for camera
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Set up camera component
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->bUsePawnControlRotation = true;
	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	UVOIPStatics::SetMicThreshold(-5);
	UE_LOG(LogTemp, Log, TEXT("Sample rate: %d"), UVOIPStatics::GetVoiceSampleRate());
	/*VOIPRef = UVOIPTalker::CreateTalkerForPlayer(GetPlayerState());
	if(VOIPRef != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Created VOIP Talker"));
	}*/
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MyCharacter: BeginPlay"), true, true, FColor::Blue, 2);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveLeftRight", this, &AMyCharacter::MoveLeftRight);
	PlayerInputComponent->BindAxis("MoveForwardBack", this, &AMyCharacter::MoveForwardBack);

	PlayerInputComponent->BindAxis("LookLeftRight", this, &AMyCharacter::LookLeftRight);
	PlayerInputComponent->BindAxis("LookUpDown", this, &AMyCharacter::LookUpDown);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::InteractInit);
}

//Provides the server with the correct camera direction
void AMyCharacter::InteractInit()
{
	Interact(PlayerCamera->GetForwardVector());
}

//Executed when the player presses the interact key (E). Only executes on server.
void AMyCharacter::Interact_Implementation(FVector CameraForwardVector)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("MyCharacter - Interact: Performing line trace"), true, true, FColor::Blue, 2);
	FHitResult Result;
	const FVector StartLocation = PlayerCamera->GetComponentLocation();
	const FVector EndLocation = (CameraForwardVector*400) + PlayerCamera->GetComponentLocation();
	FCollisionQueryParams CollisionParams(TEXT("TriggerTrace"), true, this);
	CollisionParams.bReturnPhysicalMaterial = false;
	
	GetWorld()->LineTraceSingleByChannel(Result, StartLocation, EndLocation, ECC_WorldDynamic, CollisionParams); //Performs a line trace and stores the results in Result
	DrawDebugLine(GetWorld(), StartLocation, Result.Location, FColor::Blue, true, 5); //Spawns a debugline to test
	
	if(Result.Actor == nullptr) return; //Trace didn't hit anything so end here
	ATrigger* HitTrigger = Cast<ATrigger>(Result.Actor);
	if(!HitTrigger) return; //Hit actor wasn't a trigger so end here
	
	HitTrigger->OnTrigger(this); //Hit actor was a trigger so tell it it's been triggered
}


void AMyCharacter::MoveLeftRight(float Value)
{
	if(Value) AddMovementInput(GetActorRightVector(), Value);
}
void AMyCharacter::MoveForwardBack(float Value)
{
	if(Value) AddMovementInput(GetActorForwardVector(), Value);
}
void AMyCharacter::LookLeftRight(float Value)
{
	if(Value) AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void AMyCharacter::LookUpDown(float Value)
{
	if(Value) AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

