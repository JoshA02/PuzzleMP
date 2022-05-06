// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "ButtonGrid.h"

#include "Kismet/KismetSystemLibrary.h"
#include "PuzzleMP/Trigger.h"

// Sets default values
AButtonGrid::AButtonGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	TriggerPositions.Empty();
	TriggerPositions.Add(FVector(0, -10, 60));
	TriggerPositions.Add(FVector(0, 10, 60));
	TriggerPositions.Add(FVector(0, -10, 40));
	TriggerPositions.Add(FVector(0, 10, 40));
	TriggerPositions.Add(FVector(0, 0, 19));
	
	UStaticMeshComponent* BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Body"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Meshes/L2/SM_ButtonGrid.SM_ButtonGrid"));
	if(MeshAsset.Succeeded()) BodyMesh->SetStaticMesh(MeshAsset.Object);

	SetupButtonMaterials();
	
	RootComponent = BodyMesh;
}

void AButtonGrid::SetupButtonMaterials()
{
	UStaticMeshComponent* BodyMesh = Cast<UStaticMeshComponent>(RootComponent);
	if(!BodyMesh) return;
	if(ButtonMaterials.Num() > 0) return;
	for(int x = 1; x < 6; x++)
	{
		UMaterialInstanceDynamic* Material = BodyMesh->CreateDynamicMaterialInstance(x);
		Material->SetScalarParameterValue("Number", x);
		ButtonMaterials.Add(Material);
	}
}


// Called when the game starts or when spawned
void AButtonGrid::BeginPlay()
{
	Super::BeginPlay();
	SetupButtonMaterials();

	if(!HasAuthority()) return;

	for(FVector TriggerPosition : TriggerPositions)
	{
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(TriggerPosition + GetActorLocation(), FRotator(0));
		Triggers.Add(Trigger);
		Trigger->OnTriggerDelegate.AddDynamic(this, &AButtonGrid::OnButtonPressed);
	}
}

void AButtonGrid::OnButtonPressed(AActor* TriggeringActor, AActor* TriggerActor)
{
	if(!HasAuthority()) return;
	ATrigger* Trigger = Cast<ATrigger>(TriggerActor);
	if(!Triggers.Contains(Trigger))
	{
		UE_LOG(LogTemp, Error, TEXT("Trigger not in Triggers array... somehow"));
		return;
	}
	
	const int Index = Triggers.Find(Trigger);
	const int Number = Index + 1;

	SetButtonState(Index, true);
	SetButtonStateMulticast(Index, true);
	
	UKismetSystemLibrary::PrintString(GetWorld(), (TEXT("%s"), *FString::SanitizeFloat(Number)), true, true, FColor::Blue, 2);

	AL2_Platform* Platform = GetPlatformByNumber(Number);
	if(PendingPlatforms.Contains(Platform))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Platform already pending for this sequence"), true, true, FColor::Orange, 2);
		return;
	}
	PendingPlatforms.Add(Platform);
	Platform->OnRetractDelegate.AddDynamic(this, &AButtonGrid::OnPlatformMoveComplete);
	PlatformsPending ++;
	
	if(PendingPlatforms.Num() >= 5)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Start sequence"), true, true, FColor::Blue, 2);
		StartSequence();
	}
}

void AButtonGrid::StartSequence()
{
	SequenceIndex = 0;
	GetWorldTimerManager().SetTimer(SequenceHandle, [&]
	{
		UE_LOG(LogTemp, Log, TEXT("INDEX TO PUSH OUT: %s"), *FString::SanitizeFloat(SequenceIndex));
		AL2_Platform* Platform = PendingPlatforms[SequenceIndex];
		Platform->ExtendPlatform();
		SequenceIndex ++;
		if(SequenceIndex > PendingPlatforms.Num() - 1) GetWorldTimerManager().ClearTimer(SequenceHandle);
	}, 1.0f, true, 0.0f);
}

void AButtonGrid::SetButtonStateMulticast_Implementation(int ButtonIndex, bool bNewState)
{
	SetButtonState(ButtonIndex, bNewState);
}
void AButtonGrid::SetButtonState(int ButtonIndex, bool bNewState)
{
	if(!ButtonMaterials[ButtonIndex])
	SetupButtonMaterials();

	ButtonMaterials[ButtonIndex]->SetScalarParameterValue(FName("State"), bNewState);
}

AL2_Platform* AButtonGrid::GetPlatformByNumber(int Number)
{
	for(AL2_Platform* Platform : Platforms)
		if(Platform->GetPlatformNumber() == Number) return Platform;
	
	return nullptr;
}


void AButtonGrid::OnPlatformMoveComplete() { PlatformsPending = FMath::Clamp(PlatformsPending - 1, 0, 999); }

void AButtonGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PendingPlatforms.Num() >= 5 && PlatformsPending == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("All platforms done moving"));
		for(AL2_Platform* Platform : PendingPlatforms)
			Platform->OnRetractDelegate.Clear();
		PendingPlatforms.Empty();
		for(int x = 0; x < ButtonMaterials.Num(); x++)
		{
			SetButtonState(x, false);
			SetButtonStateMulticast(x, false);
		}
	}
}
