// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "PickupableCube.h"

#include "Kismet/KismetSystemLibrary.h"

APickupableCube::APickupableCube()
{
	bReplicates = true;

	Tags.Add("pickup");
}

void APickupableCube::BeginPlay() { Super::BeginPlay(); }


void APickupableCube::OnPickup_Implementation(AActor* Player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("APickupableCube - Pickup"), true, true, FColor::Blue, 2);
	Pickup();
	PickupMulticast();
}
void APickupableCube::OnPutdown_Implementation(AActor* Player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("APickupableCube - End Pickup"), true, true, FColor::Blue, 2);
	Putdown();
	PutdownMulticast();
}
void APickupableCube::PickupMulticast_Implementation() { Pickup(); }
void APickupableCube::PutdownMulticast_Implementation() { Putdown(); }
void APickupableCube::Pickup() // Add a component variable here to attach to
{
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CubeMesh->SetEnableGravity(false);
	CubeMesh->SetSimulatePhysics(false);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void APickupableCube::Putdown()
{
    this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CubeMesh->SetEnableGravity(true);
	CubeMesh->SetSimulatePhysics(true);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}