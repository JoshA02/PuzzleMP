// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

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

