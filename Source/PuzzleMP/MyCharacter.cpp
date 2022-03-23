// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "DrawDebugHelpers.h"
#include "Trigger.h"
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

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::Interact);
}

void AMyCharacter::Interact()
{
	FHitResult Result;
	const FVector StartLocation = PlayerCamera->GetComponentLocation();
	const FVector EndLocation = (PlayerCamera->GetForwardVector()*400) + PlayerCamera->GetComponentLocation();
	FCollisionQueryParams CollisionParams(TEXT("TriggerTrace"), true, this);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = false;
	
	GetWorld()->LineTraceSingleByChannel(Result, StartLocation, EndLocation, ECC_WorldDynamic, CollisionParams);
	DrawDebugLine(GetWorld(), StartLocation, Result.Location, FColor::Blue, true, 20);
	
	if(Result.Actor == nullptr) return;
	ATrigger* HitTrigger = Cast<ATrigger>(Result.Actor);
	if(!HitTrigger)
	{
		// UE_LOG(LogTemp, Log, TEXT("Didn't hit Trigger"));
		return;
	}
		
	HitTrigger->OnTrigger(this);
	// UE_LOG(LogTemp, Log, TEXT("Hit Trigger"));
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
	if(Value)
	{
/*		const float Temp = PlayerCamera->GetRelativeRotation().Pitch + Value;
		if(Temp <= 65 && Temp >= -65)*/ AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

