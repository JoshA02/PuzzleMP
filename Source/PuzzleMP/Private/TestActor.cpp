// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"

#include "Kismet/KismetSystemLibrary.h"
#include "PuzzleMP/Trigger.h"

// Sets default values
ATestActor::ATestActor()
{
	bReplicates = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	for(int x = 0; x < 4; x++)
	{
		ATrigger* Trigger = GetWorld()->SpawnActor<ATrigger>(GetActorLocation(), GetActorRotation(), SpawnParams);
		Trigger->SetTriggerExtent(FVector(80,80,80));
		Trigger->OnTriggerDelegate.AddDynamic(this, &ATestActor::Triggered);
	}
}

void ATestActor::Triggered(AActor* TriggeringActor, AActor* TriggeredActor)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Hi"), true, true, FColor::Green, 2);
	Test();
}

void ATestActor::Test_Implementation()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Test: TEST STUFF"), true, true, FColor::Green, 2);
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

