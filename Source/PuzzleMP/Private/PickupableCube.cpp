// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "PickupableCube.h"

#include "Kismet/KismetSystemLibrary.h"

APickupableCube::APickupableCube()
{
	bReplicates = true;

	Tags.Add("pickup");
}

void APickupableCube::BeginPlay() { Super::BeginPlay(); }


// Pickup
void APickupableCube::OnPickup_Implementation(AActor* Player, USceneComponent* ToAttach)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("APickupableCube - Pickup"), true, true, FColor::Blue, 2);
	Pickup(ToAttach);
	PickupMulticast(ToAttach);
}
void APickupableCube::PickupMulticast_Implementation(USceneComponent* ToAttach) { Pickup(ToAttach); }
void APickupableCube::Pickup(USceneComponent* ToAttach) // Add a component variable here to attach to
{
	this->AttachToComponent(ToAttach, FAttachmentTransformRules::SnapToTargetIncludingScale); // Attach this cube to the new ToAttach component.
	CubeMesh->SetEnableGravity(false);
	CubeMesh->SetSimulatePhysics(false);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// Put down
void APickupableCube::OnPutdown_Implementation(AActor* Player)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("APickupableCube - End Pickup"), true, true, FColor::Blue, 2);
	Putdown();
	PutdownMulticast();
}
void APickupableCube::PutdownMulticast_Implementation() { Putdown(); }
void APickupableCube::Putdown()
{
    this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // Detach this cube from any actor it's attached to.
	CubeMesh->SetEnableGravity(true);
	CubeMesh->SetSimulatePhysics(true);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	CubeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}