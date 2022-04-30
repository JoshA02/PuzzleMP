// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "PickupableCube.h"

#include "Kismet/KismetSystemLibrary.h"

APickupableCube::APickupableCube()
{
	bReplicates = true;

	Tags.Add("pickup");
}

void APickupableCube::BeginPlay() { Super::BeginPlay(); }

void APickupableCube::OnInteract_Implementation(AActor* Player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("APickupableCube - Pickup"), true, true, FColor::Blue, 2);
	Pickup();
	PickupMulticast();
}

void APickupableCube::OnStopInteract_Implementation(AActor* Player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("APickupableCube - End Pickup"), true, true, FColor::Blue, 2);
	Putdown();
	PutdownMulticast();
}

void APickupableCube::PickupMulticast_Implementation() { Pickup(); }
void APickupableCube::PutdownMulticast_Implementation() { Putdown(); }

void APickupableCube::Pickup()
{
	CubeMesh->SetEnableGravity(false);
	CubeMesh->SetSimulatePhysics(false);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	SetActorEnableCollision(false);
}

void APickupableCube::Putdown()
{
	CubeMesh->SetEnableGravity(true);
	CubeMesh->SetSimulatePhysics(true);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	SetActorEnableCollision(true);
}