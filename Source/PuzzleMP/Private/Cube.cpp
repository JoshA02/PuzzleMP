// Fill out your copyright notice in the Description page of Project Settings.


#include "Cube.h"

#include "CubeSpawner.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACube::ACube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
	
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Cube Mesh"));
	RootComponent = CubeMesh;

	CubeMesh->SetEnableGravity(true);
	CubeMesh->SetSimulatePhysics(true);
	CubeMesh->CanCharacterStepUpOn = ECB_No;
	CubeMesh->SetMassOverrideInKg(NAME_None, 30, true);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if(CubeAsset.Succeeded()) CubeMesh->SetStaticMesh(CubeAsset.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CubeMaterialAsset(TEXT("/Game/Materials/MI_DefaultCube.MI_DefaultCube"));
	if(CubeMaterialAsset.Succeeded()) CubeMesh->SetMaterial(0, CubeMaterialAsset.Object);
	CubeMesh->SetRelativeLocation(FVector(0,0,0));
	CubeMesh->SetWorldScale3D(FVector(0.8));

	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Cube Trigger"));
	Trigger->SetBoxExtent(FVector(50));
	Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Trigger->SetCollisionObjectType(ECC_Pawn);
	Trigger->SetCollisionResponseToAllChannels(ECR_Block);
	Trigger->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void ACube::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( ACube, CurrentState );
	DOREPLIFETIME( ACube, BeingDestroyed );
	DOREPLIFETIME( ACube, Destroyed );
	DOREPLIFETIME( ACube, CubeColour );
	DOREPLIFETIME( ACube, State );
}

// Called when the game starts or when spawned
void ACube::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	CubeMaterial = CubeMesh->CreateDynamicMaterialInstance(0);
}


/* Public function used to change the CubeColour variable
 *  This executes exclusively on the server
 *  The CubeMaterial is manually updated here for the server, before being updated for the clients (within OnCubeColourChange)
*/
void ACube::ChangeCubeColour(FVector NewColour)
{
	if(!HasAuthority()) return; // Stop here if not server
	
	CubeColour = NewColour;
	if(!CubeMaterial) CubeMaterial = CubeMesh->CreateDynamicMaterialInstance(0);
	CubeMaterial->SetVectorParameterValue(FName("Colour"), NewColour);
}

/* Triggered automatically when the CubeColour variable is changed, then replicated to clients
 *	This executes exclusively on clients
*/
void ACube::OnCubeColourChange()
{
	if(!CubeMaterial) CubeMaterial = CubeMesh->CreateDynamicMaterialInstance(0);
	CubeMaterial->SetVectorParameterValue(FName("Colour"), CubeColour);
}


void ACube::Destroy()
{
	if(!HasAuthority()) return; // No authority | Requires the server to execute the destroy logic
	BeingDestroyed = true;
	State = false;
}

// Called every frame
void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!HasAuthority()) return; // Unnecessary to execute on non-server clients

	
	if( FMath::IsNearlyEqual(State, CurrentState) ) return; //Saves resources

	static float CubeStateAsFloat = State; // Turns bool into 0/1
	
	// If increasing
	if(CubeStateAsFloat > CurrentState) UpdateCurrentState( FMath::Clamp(CurrentState + (StateChangeSpeed * DeltaTime + 0.0), 0.0, 1.0) );
	// If decreasing
	if(CubeStateAsFloat < CurrentState) UpdateCurrentState( FMath::Clamp(CurrentState - (StateChangeSpeed * DeltaTime + 0.0), 0.0, 1.0) );

	if( FMath::IsNearlyEqual(State, CurrentState) )
	{
		Destroyed = !State; // Update the 'destroyed' bool to reflect new state
		if(Destroyed) GetWorld()->DestroyActor(this);
	}
}

// Executed by the server to change the CurrentState. Then triggers the visual changes for all machines
void ACube::UpdateCurrentState(const float NewState)
{
	CurrentState = NewState;
	ReflectStateChange();
}

// Auto-executes for clients when CurrentState is changed
void ACube::OnCurrentStateChange() const { ReflectStateChange(); }

// Executed by everyone when CurrentState is changed
void ACube::ReflectStateChange() const
{
	const FVector NewVelocity = CubeMesh->GetPhysicsLinearVelocity()*FMath::Lerp(0.7, 1.0, CurrentState);
	CubeMesh->SetPhysicsLinearVelocity(NewVelocity);
	if(CubeMaterial) CubeMaterial->SetScalarParameterValue("Opacity", CurrentState);
}

bool ACube::IsBeingDestroyed() { return BeingDestroyed; }
