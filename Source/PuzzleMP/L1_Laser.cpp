// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_Laser.h"

#include "Cube.h"
#include "MyCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AL1_Laser::AL1_Laser()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam Mesh"));
	RootComponent = BeamMesh;
	BeamLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Beam Light"));
	BeamLight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AL1_Laser::OnTrigger);
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AL1_Laser::BeginPlay() { Super::BeginPlay(); }

//Hides the laser and disables any overlap events - Multicast
void AL1_Laser::Disarm_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Disarm laser"));
	BeamMesh->SetVisibility(false);
	BeamLight->SetVisibility(false);
	Trigger->SetGenerateOverlapEvents(false);
}

void AL1_Laser::OnTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* Character = Cast<AMyCharacter>(OtherActor);
	ACube* Cube = Cast<ACube>(OtherActor);
	if(Character) Character->LaunchCharacter(Arrow->GetForwardVector() * LaunchStrength, false, false);
	if(Cube) Cube->Destroy();
}