// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "StandingButton.h"

#include "InteractInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PuzzleMP/Trigger.h"

// Sets default values
AStandingButton::AStandingButton()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Button Mesh"));
	RootComponent = ButtonMesh;

	TriggerLocation = CreateDefaultSubobject<USceneComponent>(FName("Trigger Position"));
	TriggerLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AStandingButton::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority()) return;
	
	Trigger = GetWorld()->SpawnActor<ATrigger>(TriggerLocation->GetComponentLocation(), TriggerLocation->GetComponentRotation());
	TriggerLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->OnTriggerDelegate.AddDynamic(this, &AStandingButton::OnButtonPressed);
	Trigger->SetTriggerExtent(FVector(15));
}

// Simply triggers the ActorToTrigger (required it implements the InteractInterface)
void AStandingButton::OnButtonPressed(AActor* TriggeringActor, AActor* TriggerActor)
{
	if(!HasAuthority()) return;
	const IInteractInterface* Interface = Cast<IInteractInterface>(ActorToTrigger);
	if(!Interface)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("StandingButton - ActorToTrigger does not implement interact interface"), true, true, FColor::Red, 2);
		return;
	}
	Interface->Execute_OnInteract(ActorToTrigger, this);
}
