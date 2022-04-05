// Fill out your copyright notice in the Description page of Project Settings.


#include "Cube.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACube::ACube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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

// Called when the game starts or when spawned
void ACube::BeginPlay()
{
	Super::BeginPlay();
	CubeMaterial = CubeMesh->CreateDynamicMaterialInstance(0);
	
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(
		UnusedHandle, this, &ACube::Destroy, 4, false);
}

void ACube::Destroy()
{
	BeingDestroyed = true;
	CubeState = false;
}

// Called every frame
void ACube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if( FMath::IsNearlyEqual(CubeState, CurrentCubeState) ) return; //Saves resources

	static float CubeStateAsFloat = CubeState; // Turns bool into 0/1
	
	// If increasing
	if(CubeStateAsFloat > CurrentCubeState) CurrentCubeState = FMath::Clamp(CurrentCubeState + (StateChangeSpeed * DeltaTime + 0.0), 0.0, 1.0);
	// If decreasing
	if(CubeStateAsFloat < CurrentCubeState) CurrentCubeState = FMath::Clamp(CurrentCubeState - (StateChangeSpeed * DeltaTime + 0.0), 0.0, 1.0);

	const FVector NewVelocity = CubeMesh->GetPhysicsLinearVelocity()*FMath::Lerp(0.7, 1.0, CurrentCubeState);
	CubeMesh->SetPhysicsLinearVelocity(NewVelocity);
	if(CubeMaterial) CubeMaterial->SetScalarParameterValue("Opacity", CurrentCubeState);

	if( FMath::IsNearlyEqual(CubeState, CurrentCubeState) ) Destroyed = !CubeState; // Update the 'destroyed' bool to reflect new state
}


