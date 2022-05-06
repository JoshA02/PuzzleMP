// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"

#include "DrawDebugHelpers.h"
#include "PickupableCube.h"
#include "Trigger.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
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

	PickupLocation = CreateDefaultSubobject<USceneComponent>(FName("Pickup Location"));
	PickupLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AMyCharacter::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( AMyCharacter, HeldItem ); // Sets up replication for the HeldItem
	DOREPLIFETIME( AMyCharacter, HeldItemPosition ); // Sets up replication for the desired position of the HeldItem
	DOREPLIFETIME( AMyCharacter, CachedForwardVec );
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	UVOIPStatics::SetMicThreshold(-5);
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
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AMyCharacter::StopInteractInit);
}

void AMyCharacter::Server_UpdateCachedForwardVec_Implementation(FVector ForwardVector) { CachedForwardVec = ForwardVector; }

//Provides the server with the correct camera direction
void AMyCharacter::InteractInit() { Interact(PlayerCamera->GetForwardVector()); }

void AMyCharacter::StopInteractInit() { StopInteract(); }

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
	// DrawDebugLine(GetWorld(), StartLocation, Result.Location, FColor::Blue, true, 5); //Spawns a debugline to test
	
	if(Result.Actor == nullptr) return; //Trace didn't hit anything so end here
	AActor* Actor = Cast<AActor>(Result.Actor);
	if(Actor == nullptr) return;

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(Result.Actor);
	if(PickupInterface) // If the interacted object was a 'pickupable' object...
	{
		PickupInterface->Execute_OnPickup(Actor, this, PickupLocation);
		HeldItem = Actor;
		return;
	}
	
	ATrigger* HitTrigger = Cast<ATrigger>(Result.Actor);
	if(HitTrigger) // If the interacted object was a trigger...
	{
		HitTrigger->OnTrigger(this); // Hit actor was a trigger so tell it it's been triggered
		return;
	}
}

void AMyCharacter::StopInteract_Implementation()
{
	IPickupInterface* Interface = Cast<IPickupInterface>(HeldItem);
	if(Interface == nullptr) return;

	Interface->Execute_OnPutdown(HeldItem, this);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	/* Held Item Stuff
	 * This section moves the (replicated) HeldItem to the (replicated) HeldItemPosition.
	*/

	if(!HeldItem) return;

	
	// if(HeldItem->GetAttachParentActor() != this)
	if(!HeldItem->IsAttachedTo(this))
	{
		UE_LOG(LogTemp, Log, TEXT("HeldItem is no longer attached, set HeldItem to null"));
		HeldItem = nullptr;
	}
	
	if(HeldItem == nullptr) return;
	if(HeldItem->IsPendingKill()) return;
	if(HeldItem->IsActorBeingDestroyed()) return;

	// From here, held item is valid
	
	// If the server is not controlling this character and not running this code
	if(!HasAuthority() && IsLocallyControlled()) Server_UpdateCachedForwardVec(PlayerCamera->GetForwardVector()); // Tell the server its forward vector (where it's facing)

	// If the server is controlling this character and is running this code
	if(HasAuthority() && IsLocallyControlled()) CachedForwardVec = PlayerCamera->GetForwardVector();
	
	PickupLocation->SetWorldLocation(PlayerCamera->GetComponentLocation() + (CachedForwardVec * HoldDistance) + FVector(0, 0, -50));
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

AActor* AMyCharacter::GetHeldItem() { return HeldItem; }

