// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
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

void ADoor::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( ADoor, State );
	DOREPLIFETIME( ADoor, CurrentState );
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	DoorMaterial = LeftDoor->CreateDynamicMaterialInstance(0);
	RightDoor->SetMaterial(0, DoorMaterial);
	DoorMaterial->SetScalarParameterValue("DoorState", 0);
}

void ADoor::CloseDoors() { State = 0; }
void ADoor::OpenDoors() { State = 1; }

void ADoor::OnInteract_Implementation(AActor* Caller)
{
	if(!HasAuthority()) return; //Only allow door to be opened by the server
	UE_LOG(LogTemp, Log, TEXT("Open door"));
	OpenDoors();
}

void ADoor::OnStopInteract_Implementation(AActor* Caller)
{
	if(!HasAuthority()) return; //Only allow door to be closed by the server
	UE_LOG(LogTemp, Log, TEXT("Close door"));
	CloseDoors();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!HasAuthority()) return; // Unnecessary to execute on non-server clients
	
	if(CurrentState == State) return;

	// If increasing
	if(State > CurrentState) UpdateCurrentState( FMath::Clamp(CurrentState + (StateChangeSpeed*DeltaTime + 0.0), 0.0, 1.0) );
	// If decreasing
	if(State < CurrentState) UpdateCurrentState( FMath::Clamp(CurrentState - (StateChangeSpeed*DeltaTime + 0.0), 0.0, 1.0) );
}

// Executed by the server to change the CurrentState. Then triggers the visual changes for all machines
void ADoor::UpdateCurrentState(const float NewState)
{
	CurrentState = NewState;
	ReflectStateChange();
}

// Auto-executes for clients when CurrentState is changed
void ADoor::OnCurrentStateChange() const { ReflectStateChange(); }

// Executed by everyone when CurrentState is changed
void ADoor::ReflectStateChange() const
{
	constexpr float OpenWidth = 110;
	LeftDoor->SetRelativeLocation(InitialLocation + FVector(FMath::Lerp(0.0f, OpenWidth, CurrentState),0,0));
	RightDoor->SetRelativeLocation(InitialLocation - FVector(FMath::Lerp(0.0f, OpenWidth, CurrentState),0,0));
	if(DoorMaterial) DoorMaterial->SetScalarParameterValue("LockState", CurrentState);
}


