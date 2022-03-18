// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_Laser.h"

#include "MyCharacter.h"
#include "Components/LightComponent.h"

// Sets default values
AL1_Laser::AL1_Laser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BeamMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam Mesh"));
	RootComponent = BeamMesh;
	BeamLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Beam Light"));
	BeamLight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger"));
	Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AL1_Laser::OnTrigger);

	// BeamLight->SetLightColor(FLinearColor(255, 49, 57, 255));
	// BeamLight->SetIntensityUnits(ELightUnits::Candelas);
}

// Called when the game starts or when spawned
void AL1_Laser::BeginPlay()
{
	Super::BeginPlay();
	
}

void AL1_Laser::Disarm()
{
	UE_LOG(LogTemp, Log, TEXT("Disarm laser"));
	BeamMesh->SetVisibility(false);
	BeamLight->SetVisibility(false);
	Trigger->SetVisibility(false);
}

void AL1_Laser::OnTrigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->GetClass() == AMyCharacter::StaticClass()){
		UE_LOG(LogTemp, Log, TEXT("Player overlapped laser"));
		//Kill player
	}
}


// Called every frame
void AL1_Laser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

