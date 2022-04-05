// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeButton.h"

#include "Cube.h"
#include "InteractInterface.h"
#include "GeometryCollection/GeometryCollectionSimulationTypes.h"

// Sets default values
ACubeButton::ACubeButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Button Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/General/SM_CubeButton.SM_CubeButton"));
	if(MeshAsset.Succeeded()) Mesh->SetStaticMesh(MeshAsset.Object);
	RootComponent = Mesh;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(FName("Trigger Box"));
	TriggerBox->SetBoxExtent(FVector(5, 5, 10));
	TriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TriggerBox->SetRelativeLocation(FVector(0, 0, 10));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetCollisionObjectType(ECC_Pawn);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACubeButton::TriggerOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ACubeButton::TriggerStop);
}

void ACubeButton::TriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(RequiredPresserType && !OtherActor->GetClass()->IsChildOf(RequiredPresserType)) return;
	
	IInteractInterface* Interface = Cast<IInteractInterface>(PoweredActor);
	if(!Interface) return;
	Interface->Execute_OnInteract(PoweredActor, this);
}

void ACubeButton::TriggerStop(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(RequiredPresserType && !OtherActor->GetClass()->IsChildOf(RequiredPresserType)) return;
	
	IInteractInterface* Interface = Cast<IInteractInterface>(PoweredActor);
	if(!Interface) return;
	Interface->Execute_OnStopInteract(PoweredActor, this);
}

void ACubeButton::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}


// Called when the game starts or when spawned
void ACubeButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

