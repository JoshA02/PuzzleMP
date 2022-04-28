// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "SaveTriggerActor.h"

#include "SavingUtils.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASaveTriggerActor::ASaveTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Box Trigger"));
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASaveTriggerActor::OnPlayerOverlap);

}

// Called when the game starts or when spawned
void ASaveTriggerActor::BeginPlay()
{
	Super::BeginPlay();
}

// Executed automatically when a player overlaps the trigger
void ASaveTriggerActor::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return; // Only continue if host

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Overlapped SaveTrigger"), true, true, FColor::Blue, 2);
	
	const int CurrentlySavedLevel = SavingUtils::GetSavedLevel();
	if( CurrentlySavedLevel > LevelInt ) return; // If the player's saved level is greater than this level, don't overwrite it.
	SavingUtils::SetSavedLevel( LevelInt ); // Update the saved level to the current one.
}



// Called every frame
void ASaveTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

