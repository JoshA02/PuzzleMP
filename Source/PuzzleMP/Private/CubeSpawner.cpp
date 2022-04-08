// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeSpawner.h"

#include "Cube.h"

// Sets default values
ACubeSpawner::ACubeSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/General/SM_CubeSpawner.SM_CubeSpawner"));
	if(MeshAsset.Succeeded()) Mesh->SetStaticMesh(MeshAsset.Object);
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ACubeSpawner::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority()) return;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &ACubeSpawner::SpawnObject, 5, true);
}

void ACubeSpawner::SpawnObject()
{
	if(!HasAuthority()) return;

	// Removes any invalid cubes from the array
	for(int x = 0; x < Cubes.Num(); x++)
		if(!Cubes[x] || Cubes[x]->IsBeingDestroyed() || Cubes[x]->IsActorBeingDestroyed()) Cubes.RemoveAt(x);

	// Destroy the oldest cube if the spawner has hit the limit
	if(MaxCubes > 0 && Cubes.Num() >= MaxCubes) Cubes[0]->Destroy();

	const FVector SpawnLoc = GetActorLocation() + FVector(0,0,200);
	Cubes.Add( GetWorld()->SpawnActor<ACube>(SpawnLoc, FRotator(0)) );
	
	// Call a multicast function for things like sounds that want to be played for all players.
}

void ACubeSpawner::OnInteract_Implementation(AActor* Caller) { SpawnObject(); }

// Called every frame
void ACubeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

