// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "L2_Platform.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AL2_Platform::AL2_Platform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ClosedLocation = CreateDefaultSubobject<USceneComponent>(FName("Closed Location"));
	ClosedLocation->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AL2_Platform::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME( AL2_Platform, TargetAlpha );
	DOREPLIFETIME( AL2_Platform, MoveState );
	DOREPLIFETIME( AL2_Platform, bDoneMoving );
}

// Called when the game starts or when spawned
void AL2_Platform::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AL2_Platform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(TargetAlpha > Alpha)
	{
		Alpha = FMath::Clamp(Alpha + (DeltaTime * 1.6f), 0.0f, 1.0f); // Extend at 1.6x DeltaTime
	}
	if(TargetAlpha < Alpha)
	{
		Alpha = FMath::Clamp(Alpha - (DeltaTime * 1.8f), 0.0f, 1.0f); // Retract at 1.8x DeltaTime (faster than extending)
	}

	Mesh->SetWorldLocation(FMath::Lerp(this->GetActorLocation(), ClosedLocation->GetComponentLocation(),  Alpha));

	
	if(HasAuthority()){
		if(Alpha == TargetAlpha) // If platform is stationary
		{
			if(!bDoneMoving) // If bDoneMoving bool is false, set it to true. This means this logic will only get executed once, until reset back to false
			{
				bDoneMoving = true;
				if(TargetAlpha == 1.0f && Alpha == TargetAlpha)
				{
					UE_LOG(LogTemp, Log, TEXT("Done moving out"));
					FTimerHandle UnusedHandle;
					GetWorldTimerManager().SetTimer(UnusedHandle, [&]
					{
						UE_LOG(LogTemp, Log, TEXT("Retracting platform"));
						RetractPlatform();
					}, 0.1f, false, 4.0f);
				}
				if(TargetAlpha == 0.0f && Alpha == TargetAlpha)
				{
					UE_LOG(LogTemp, Log, TEXT("Done moving in"));
					OnRetractDelegate.Broadcast();
				}
			}
		}
		if(Alpha != TargetAlpha) bDoneMoving = false;
	}
}

int AL2_Platform::GetPlatformNumber() { return PlatformNumber; }

void AL2_Platform::ExtendPlatform()
{
	if(!HasAuthority()) return;
	TargetAlpha = 1.0f;
}
void AL2_Platform::RetractPlatform()
{
	if(!HasAuthority()) return;
	TargetAlpha = 0.0f;
}

bool AL2_Platform::IsDoneMoving() { return bDoneMoving; }
int AL2_Platform::GetMoveState() { return MoveState; }



