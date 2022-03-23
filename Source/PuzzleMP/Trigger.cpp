// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger.h"

// Sets default values
ATrigger::ATrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
void ATrigger::BeginPlay()
{
	Super::BeginPlay();
}

void ATrigger::SetTriggerExtent(FVector NewExtent)
{
	TriggerExtent = NewExtent;
	Trigger->SetBoxExtent(FVector(TriggerExtent.X,TriggerExtent.Y,TriggerExtent.Z), true);
}

void ATrigger::OnTrigger(AActor* TriggeringActor)
{
	UE_LOG(LogTemp, Log, TEXT("Actor triggered trigger"));
	OnTriggerDelegate.Broadcast(TriggeringActor);
}


// Called every frame
void ATrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if(this->GetOwner() != nullptr) UE_LOG(LogTemp, Display, TEXT("Owner exists"));
	// if(this->GetOwner() == nullptr) UE_LOG(LogTemp, Display, TEXT("Owner doesn't exist"));
}
