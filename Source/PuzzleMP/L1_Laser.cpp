// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_Laser.h"

#include "MyCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AL1_Laser::AL1_Laser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
	if(Character == nullptr) return;
	Character->LaunchCharacter(Arrow->GetForwardVector() * LaunchStrength, false, false);
}


// Called every frame
void AL1_Laser::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

