// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "StandingButton.h"

#include "InteractInterface.h"
#include "Kismet/GameplayStatics.h"
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

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> ButtonPressSoundAtt (TEXT("/Game/Audio/ButtonAttenuation.ButtonAttenuation"));
	if(ButtonPressSoundAtt.Succeeded()) ButtonPressSoundAttenuation = ButtonPressSoundAtt.Object;
	static ConstructorHelpers::FObjectFinder<USoundBase> ButtonPressSoundAsset(TEXT("/Game/Audio/Wavs/sfx_3p_buttonpress.sfx_3p_buttonpress"));
	if(ButtonPressSoundAsset.Succeeded()) PressSound = ButtonPressSoundAsset.Object;

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
	PlayButtonSoundMulticast();
	if(!HasAuthority()) return;
	const IInteractInterface* Interface = Cast<IInteractInterface>(ActorToTrigger);
	if(!Interface)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("StandingButton - ActorToTrigger does not implement interact interface"), true, true, FColor::Red, 2);
		return;
	}
	Interface->Execute_OnInteract(ActorToTrigger, this);
}

void AStandingButton::PlayButtonSoundMulticast_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PressSound, GetActorLocation(), FRotator(0), 1, 1, 0, ButtonPressSoundAttenuation);
}