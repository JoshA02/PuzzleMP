// Copyright 2022-2022 Josh Villyat. All Rights Reserved


#include "OverlapTrigger.h"

#include "InteractInterface.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AOverlapTrigger::AOverlapTrigger()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("Trigger"));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AOverlapTrigger::OnTrigger);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AOverlapTrigger::OnStopTrigger);
}

// Called when the game starts or when spawned
void AOverlapTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AOverlapTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AOverlapTrigger::OnTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Overlap"), true, true, FColor::Blue, 2);
	const IInteractInterface* InteractInterface = Cast<IInteractInterface>(ActorToTrigger);
	if(InteractInterface) InteractInterface->Execute_OnInteract(ActorToTrigger, this);
}

void AOverlapTrigger::OnStopTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Stop Overlap"), true, true, FColor::Blue, 2);
	const IInteractInterface* InteractInterface = Cast<IInteractInterface>(ActorToTrigger);
	if(InteractInterface) InteractInterface->Execute_OnStopInteract(ActorToTrigger, this);
}

