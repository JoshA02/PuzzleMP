// Fill out your copyright notice in the Description page of Project Settings.


#include "L1_ButtonPanel.h"

#include "MyPlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AL1_ButtonPanel::AL1_ButtonPanel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonPanel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_ButtonPanel"));
	ButtonPanel->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AL1_ButtonPanel::BeginPlay()
{
	Super::BeginPlay();
	if(Laser == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Laser actor not provided"))
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Specified laser found"))

	AMyPlayerController* HostPlayerController;
	//Get host
	if(HasAuthority()) HostPlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Cannot find host player. Puzzle cannot continue"));
		return;
	}

	//
}

// Called every frame
void AL1_ButtonPanel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

