// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger.h"

#include "GameplayDebuggerTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATrigger::ATrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(FName("TriggerBox"));
	Trigger->SetCollisionObjectType(ECC_WorldDynamic);
	Trigger->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	RootComponent = Trigger;
}

void ATrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Trigger->SetBoxExtent(FVector(TriggerExtent.X,TriggerExtent.Y,TriggerExtent.Z), true);
}

// Called when the game starts or when spawned
void ATrigger::BeginPlay() { Super::BeginPlay(); }

void ATrigger::SetTriggerExtent(FVector NewExtent)
{
	TriggerExtent = NewExtent;
	Trigger->SetBoxExtent(FVector(TriggerExtent.X,TriggerExtent.Y,TriggerExtent.Z), true);
}

//OnTrigger: Executed by the server from MyCharacter.cpp.
void ATrigger::OnTrigger(AActor* TriggeringActor)
{
	if(!Enabled)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnTrigger: Trigger Disabled"), true, true, FColor::Red, 2);
		return;
	}
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("OnTrigger: Triggered"), false, true, FColor::Blue, 2);
	OnTriggerDelegate.Broadcast(TriggeringActor, this);
}


//TODO: Have separate multicast function for post-trigger effects (like deleting the trigger if single use).




// Called every frame
void ATrigger::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
