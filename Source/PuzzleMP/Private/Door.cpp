// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

#include <string>
#include <ThirdParty/openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathFun.h>

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	
	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(FName("Left Door"));
	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(FName("Right Door"));
	LeftDoor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RightDoor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/General/SM_Door_Left.SM_Door_Left"));
	if(MeshAsset.Succeeded()) LeftDoor->SetStaticMesh(MeshAsset.Object);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetRight(TEXT("/Game/Meshes/General/SM_Door_Right.SM_Door_Right"));
	if(MeshAssetRight.Succeeded()) RightDoor->SetStaticMesh(MeshAssetRight.Object);

	InitialLocation = LeftDoor->GetRelativeLocation();
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	DoorMaterial = LeftDoor->CreateDynamicMaterialInstance(0);
	RightDoor->SetMaterial(0, DoorMaterial);
	DoorMaterial->SetScalarParameterValue("DoorState", 0);
}

void ADoor::CloseDoors()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Close Door"), true, true, FColor::Red, 2);
	DoorState = 0;
}
void ADoor::OpenDoors()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Open Door"), true, true, FColor::Red, 2);
	DoorState = 1;
}


void ADoor::OnInteract_Implementation(AActor* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("Open door"));
	OpenDoors();
}

void ADoor::OnStopInteract_Implementation(AActor* Caller)
{
	UE_LOG(LogTemp, Log, TEXT("Close door"));
	CloseDoors();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CurrentDoorState == DoorState) return;

	// If increasing
	if(DoorState > CurrentDoorState) CurrentDoorState = FMath::Clamp(CurrentDoorState + (StateChangeSpeed*DeltaTime + 0.0), 0.0, 1.0);
	// If decreasing
	if(DoorState < CurrentDoorState) CurrentDoorState = FMath::Clamp(CurrentDoorState - (StateChangeSpeed*DeltaTime + 0.0), 0.0, 1.0);
	
	constexpr float OpenWidth = 110;
	LeftDoor->SetRelativeLocation(InitialLocation + FVector(FMath::Lerp(0.0f, OpenWidth, CurrentDoorState),0,0));
	RightDoor->SetRelativeLocation(InitialLocation - FVector(FMath::Lerp(0.0f, OpenWidth, CurrentDoorState),0,0));
	if(DoorMaterial) DoorMaterial->SetScalarParameterValue("LockState", CurrentDoorState);
}

